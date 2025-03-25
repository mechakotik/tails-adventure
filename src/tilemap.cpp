#include "tilemap.h"
#include <error.h>
#include <algorithm>
#include <sstream>
#include <tmxpp.hpp>
#include "character.h"
#include "resource_manager.h"
#include "tools.h"

void TA_Tilemap::load(std::string filename) {
    this->filename = filename;
    tmx::Map map;
    try {
        map.parseFromData(TA::resmgr::loadAsset(filename));
    } catch(std::exception& e) {
        TA::handleError("failed to load %s: %s", filename.c_str(), e.what());
    }

    width = map.width();
    height = map.height();
    layerCount = map.layers().size();
    tileWidth = map.tileWidth();
    tileHeight = map.tileHeight();

    if(map.tilesets().size() != 1) {
        TA::handleError("failed to load %s: multiple tilesets are not supported", filename.c_str());
    }
    if(map.tilesets()[0].source() != "") {
        TA::handleError("failed to load %s: external tilesets are not supported", filename.c_str());
    }

    tilemap.resize(layerCount);
    for(int layer = 0; layer < layerCount; layer++) {
        tilemap[layer].resize(width);
        for(int pos = 0; pos < width; pos++) {
            tilemap[layer][pos].resize(height);
        }
    }

    loadTileset(map.tilesets()[0]);
    for(int pos = 0; pos < map.layers().size(); pos++) {
        loadLayer(pos, map.layers()[pos]);
    }

    borderPolygons[0].setRectangle(TA_Point(0, -16), TA_Point(width * tileWidth, 0));
    borderPolygons[1].setRectangle(TA_Point(-16, 0), TA_Point(0, height * tileHeight));
    borderPolygons[2].setRectangle(
        TA_Point(width * tileWidth, 0), TA_Point(width * tileWidth + 16, height * tileHeight));

    if(collisionLayers.empty()) {
        collisionLayers.push_back(0);
    }
}

void TA_Tilemap::loadTileset(const tmx::Tileset& tiles) {
    tileset.assign(tiles.tileCount(), Tile());
    std::filesystem::path textureFilename = filename.parent_path() / tiles.image().source();

    for(size_t tile = 0; tile < tiles.tileCount(); tile += 1) {
        tileset[tile].sprite.load(textureFilename, tileWidth, tileHeight);
        tileset[tile].sprite.setFrame(tile);
    }

    for(const tmx::Tile& tile : tiles.tiles()) {
        if(!tile.animation().empty()) {
            TA_Animation animation;
            int delayMs = tile.animation().at(0).duration;
            animation.delay = static_cast<int>((static_cast<float>(delayMs) * 60 / 1000) + 0.5);
            animation.frames.resize(tile.animation().size());
            for(int i = 0; i < tile.animation().size(); i++) {
                animation.frames[i] = tile.animation().at(i).id;
            }
            tileset[tile.id()].sprite.setAnimation(animation);
        }

        if(tile.hasProperty("spikes")) {
            tileset[tile.id()].hitboxes = getSpikesHitboxVector(tile.property("spikes").intValue());
        }

        if(tile.objectGroup().objects().empty()) {
            continue;
        }
        if(tile.objectGroup().objects().size() > 1) {
            TA::printWarning("%s", "multiple objects in tile are not supported, using the first object");
        }
        tmx::Object object = tile.objectGroup().objects()[0];
        if(object.type() != tmx::Object::Type::POLYGON) {
            TA::printWarning("%s", "object shape is not polygon, ignoring it");
            continue;
        }

        TA_Point start{object.position().x, object.position().y};
        TA_Polygon polygon;
        for(int i = 0; i < object.polygon().size(); i++) {
            TA_Point cur{object.polygon()[i].x, object.polygon()[i].y};
            polygon.addVertex(start + cur);
        }

        if(object.hasProperty("type")) {
            tileset[tile.id()].type = object.property("type").intValue();
        }
        int collisionType = TA_COLLISION_SOLID;
        if(tileset[tile.id()].type == 1) {
            collisionType = TA_COLLISION_SOLID_UP;
        } else if(tileset[tile.id()].type == 2) {
            collisionType = TA_COLLISION_SOLID | TA_COLLISION_DAMAGE;
        } else if(tileset[tile.id()].type == 3) {
            collisionType = TA_COLLISION_WATER;
        } else if(tileset[tile.id()].type == 4) {
            collisionType = TA_COLLISION_SOLID_DOWN;
        }
        tileset[tile.id()].hitboxes.push_back({polygon, collisionType});
    }
}

void TA_Tilemap::loadLayer(int id, const tmx::Layer& layer) {
    if(layer.type() != tmx::Layer::Type::TILE) {
        TA::printWarning("%s", "layer is not tile layer, ignoring it");
        return;
    }
    for(int x = 0; x < width; x++) {
        for(int y = 0; y < height; y++) {
            tilemap[id][x][y] = layer.tileLayer().at(x, y) - 1;
        }
    }
    if(layer.tileLayer().hasProperty("collision") && layer.tileLayer().property("collision").boolValue()) {
        collisionLayers.push_back(id);
    }
}

void TA_Tilemap::draw(int priority) {
    auto drawLayer = [&](int layer) {
        int lx = 0, rx = width - 1, ly = 0, ry = height - 1;
        if(camera != nullptr && TA::equal(position.x, 0) && TA::equal(position.y, 0)) {
            TA_Point cameraPos = camera->getPosition();
            lx = std::max(0, static_cast<int>(cameraPos.x / tileWidth));
            rx = std::min(width - 1, static_cast<int>((cameraPos.x + TA::screenWidth) / tileWidth));
            ly = std::max(0, static_cast<int>(cameraPos.y / tileWidth));
            ry = std::min(height - 1, static_cast<int>((cameraPos.y + TA::screenHeight) / tileWidth));
        }

        for(int tileX = lx; tileX <= rx; tileX++) {
            for(int tileY = ly; tileY <= ry; tileY++) {
                if(tilemap[layer][tileX][tileY] != -1) {
                    TA_Sprite& sprite = tileset[tilemap[layer][tileX][tileY]].sprite;
                    sprite.setPosition(position + TA_Point(tileX * tileWidth, tileY * tileHeight));
                    sprite.draw();
                }
            }
        }
    };

    if(priority == 0) {
        if(updateAnimation) {
            for(size_t tile = 0; tile < tileset.size(); tile += 1) {
                tileset[tile].sprite.setUpdateAnimation(true);
                tileset[tile].sprite.updateAnimation();
                tileset[tile].sprite.setUpdateAnimation(false);
            }
        }
        for(int layer = 0; layer < std::max(1, layerCount - 1); layer++) {
            drawLayer(layer);
        }
    } else if(priority == 1 && layerCount >= 2) {
        drawLayer(layerCount - 1);
    }
}

void TA_Tilemap::setCamera(TA_Camera* newCamera) {
    camera = newCamera;
    newCamera->setBorder({TA_Point(0, 0), TA_Point(width * tileWidth, height * tileHeight)});
    for(size_t tile = 0; tile < tileset.size(); tile += 1) {
        tileset[tile].sprite.setCamera(newCamera);
    }
}

void TA_Tilemap::setPosition(TA_Point position) {
    this->position = position;
}

int TA_Tilemap::checkCollision(TA_Polygon& polygon) {
    int minX = 1e5, maxX = 0, minY = 1e5, maxY = 0;

    for(int pos = 0; pos < polygon.size(); pos++) {
        TA_Point vertex = polygon.getVertex(pos);
        minX = std::min(minX, int(vertex.x / tileWidth));
        maxX = std::max(maxX, int(vertex.x / tileWidth));
        minY = std::min(minY, int(vertex.y / tileHeight));
        maxY = std::max(maxY, int(vertex.y / tileHeight));
    }

    auto normalize = [&](int& value, int left, int right) {
        value = std::max(value, left);
        value = std::min(value, right);
    };

    normalize(minX, 0, width - 1);
    normalize(maxX, 0, width - 1);
    normalize(minY, 0, height - 1);
    normalize(maxY, 0, height - 1);

    int flags = 0;

    auto checkCollisionWithTile = [&](int layer, int tileX, int tileY) {
        int tileId = tilemap[layer][tileX][tileY];
        if(tileId == -1) {
            return;
        }

        for(auto& hitbox : tileset[tileId].hitboxes) {
            hitbox.polygon.setPosition(TA_Point(tileX * tileWidth, tileY * tileHeight));
            if(polygon.intersects(hitbox.polygon)) {
                flags |= hitbox.type;
            }
        }
    };

    for(int layer : collisionLayers) {
        for(int tileX = minX; tileX <= maxX; tileX++) {
            for(int tileY = minY; tileY <= maxY; tileY++) {
                checkCollisionWithTile(layer, tileX, tileY);
            }
        }
    }

    for(int pos = 0; pos < (int)borderPolygons.size(); pos++) {
        if(borderPolygons[pos].intersects(polygon)) {
            flags |= TA_COLLISION_SOLID;
        }
    }

    return flags;
}

void TA_Tilemap::setUpdateAnimation(bool enabled) {
    updateAnimation = enabled;
}

std::vector<TA_Tilemap::Hitbox> TA_Tilemap::getSpikesHitboxVector(int type) {
    std::vector<Hitbox> hitboxes;
    hitboxes.push_back(getSpikesSolidHitbox(type));
    hitboxes.push_back(getSpikesDamageHitbox(type));
    return hitboxes;
}

TA_Tilemap::Hitbox TA_Tilemap::getSpikesSolidHitbox(int type) {
    TA_Point topLeft, bottomRight;
    switch(type) {
        case 0:
            topLeft = {0, 1};
            bottomRight = {16, 16};
            break;
        case 1:
            topLeft = {0, 0};
            bottomRight = {16, 15};
            break;
        case 2:
            topLeft = {1, 0};
            bottomRight = {16, 16};
            break;
        case 3:
            topLeft = {0, 0};
            bottomRight = {15, 16};
    }

    TA_Polygon polygon;
    polygon.setRectangle(topLeft, bottomRight);
    return {polygon, TA_COLLISION_SOLID};
}

TA_Tilemap::Hitbox TA_Tilemap::getSpikesDamageHitbox(int type) {
    TA_Point topLeft, bottomRight;
    switch(type) {
        case 0:
            topLeft = {0.01, 0.99};
            bottomRight = {15.99, 1};
            break;
        case 1:
            topLeft = {0.01, 15};
            bottomRight = {15.99, 15.01};
            break;
        case 2:
            topLeft = {0.99, 0.01};
            bottomRight = {1, 15.99};
            break;
        case 3:
            topLeft = {15, 0.01};
            bottomRight = {15.01, 15.99};
            break;
    }

    TA_Polygon polygon;
    polygon.setRectangle(topLeft, bottomRight);
    return {polygon, TA_COLLISION_DAMAGE};
}
