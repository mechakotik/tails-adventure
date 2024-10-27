#include <algorithm>
#include <sstream>
#include "SDL3/SDL.h"
#include "tilemap.h"
#include "tinyxml2.h"
#include "error.h"
#include "filesystem.h"
#include "resource_manager.h"
#include "tools.h"
#include "character.h"

void TA_Tilemap::load(std::string filename) // TODO: rewrite this with TMX parser
{
    tinyxml2::XMLDocument xmlFile;
    xmlFile.Parse(TA::resmgr::loadAsset(filename).c_str());
    tinyxml2::XMLElement *xmlRoot = xmlFile.FirstChildElement("map");
    
    width = xmlRoot->IntAttribute("width");
    height = xmlRoot->IntAttribute("height");
    layerCount = xmlRoot->IntAttribute("nextlayerid") - 1;
    tileWidth = xmlRoot->FirstChildElement("tileset")->IntAttribute("tilewidth");
    tileHeight = xmlRoot->FirstChildElement("tileset")->IntAttribute("tileheight");
    int tileCount = xmlRoot->FirstChildElement("tileset")->IntAttribute("tilecount");

    tilemap.resize(layerCount);
    for(int layer = 0; layer < layerCount; layer ++) {
        tilemap[layer].resize(width);
        for(int pos = 0; pos < width; pos ++) {
            tilemap[layer][pos].resize(height);
        }
    }
    tileset.assign(tileCount, Tile());

    auto loadTileset = [&](tinyxml2::XMLElement *tilesetElement)
    {
        std::string textureFilename = filename;
        while(!textureFilename.empty() && textureFilename.back() != '/') {
            textureFilename.pop_back();
        }
        textureFilename += tilesetElement->FirstChildElement("image")->Attribute("source");

        for(size_t tile = 0; tile < tileCount; tile += 1) {
            tileset[tile].sprite.load(textureFilename, tileWidth, tileHeight);
            tileset[tile].sprite.setFrame(tile);
        }

        for(tinyxml2::XMLElement *tileElement = tilesetElement->FirstChildElement("tile");
            tileElement != nullptr; tileElement = tileElement->NextSiblingElement("tile"))
        {
            int tileId = tileElement->IntAttribute("id");

            if(tileElement->FirstChildElement("animation") != nullptr) {
                TA_Animation animation;
                int delayMs = tileElement->FirstChildElement("animation")->FirstChildElement("frame")->IntAttribute("duration");
                animation.delay = int(delayMs * 60 / 1000 + 0.5);
                for(tinyxml2::XMLElement *frameElement = tileElement->FirstChildElement("animation")->FirstChildElement("frame");
                    frameElement != nullptr; frameElement = frameElement->NextSiblingElement("frame")) {
                    animation.frames.push_back(frameElement->IntAttribute("tileid"));
                }
                tileset[tileId].sprite.setAnimation(animation);
            }

            if(tileElement->FirstChildElement("objectgroup") != nullptr) {
                tinyxml2::XMLElement *object = tileElement->FirstChildElement("objectgroup")->FirstChildElement("object");
                std::stringstream pointStream;
                pointStream << object->FirstChildElement("polygon")->Attribute("points");
                TA_Point currentPoint, startPoint;
                startPoint.x = object->IntAttribute("x");
                startPoint.y = object->IntAttribute("y");

                TA_Polygon polygon;
                char temp;

                while(pointStream >> currentPoint.x) {
                    pointStream >> temp >> currentPoint.y;
                    polygon.addVertex(currentPoint + startPoint);
                }
                if(object->FirstChildElement("properties") != nullptr) {
                    tileset[tileId].type = object->FirstChildElement("properties")->FirstChildElement("property")->IntAttribute("value");
                }

                int collisionType = TA_COLLISION_SOLID;
                if(tileset[tileId].type == 1) {
                    collisionType = TA_COLLISION_HALF_SOLID;
                }
                else if(tileset[tileId].type == 2) {
                    collisionType = TA_COLLISION_SOLID | TA_COLLISION_DAMAGE;
                }
                else if(tileset[tileId].type == 3) {
                    collisionType = TA_COLLISION_WATER;
                }
                tileset[tileId].hitboxes.push_back({polygon, collisionType});
            }

            if(tileElement->FirstChildElement("properties") != nullptr) {
                int type = tileElement->FirstChildElement("properties")->FirstChildElement("property")->IntAttribute("value");
                tileset[tileId].hitboxes = getSpikesHitboxVector(type);
            }
        }
    };

    int currentLayer = 0;

    auto loadLayer = [&](tinyxml2::XMLElement *layerElement)
    {
        int layer = currentLayer;
        currentLayer ++;
        std::stringstream mapStream;
        mapStream << layerElement->FirstChildElement("data")->GetText();
        for(int tileY = 0; tileY < height; tileY ++ ) {
            for(int tileX = 0; tileX < width; tileX ++) {
                int tile;
                char temp;
                mapStream >> tile;
                if(tileX != width - 1 || tileY != height - 1) {
                    mapStream >> temp;
                }
                tilemap[layer][tileX][tileY] = tile - 1;
            }
        }

        tinyxml2::XMLElement *propertyElement = layerElement->FirstChildElement("properties");
        if(propertyElement != nullptr) {
            propertyElement = propertyElement->FirstChildElement("property");
            if(propertyElement != nullptr && std::strcmp(propertyElement->Attribute("name"), "collision") == 0) {
                collisionLayers.push_back(layer);
            }
        }
    };

    loadTileset(xmlRoot->FirstChildElement("tileset"));
    tinyxml2::XMLElement *layerElement = xmlRoot->FirstChildElement("layer");
    for(int layer = 0; layer < layerCount; layer ++) {
        loadLayer(layerElement);
        layerElement = layerElement->NextSiblingElement("layer");
    }

    borderPolygons[0].setRectangle(TA_Point(0, -16), TA_Point(width * tileWidth, 0));
    borderPolygons[1].setRectangle(TA_Point(-16, 0), TA_Point(0, height * tileHeight));
    borderPolygons[2].setRectangle(TA_Point(width * tileWidth, 0), TA_Point(width * tileWidth + 16, height * tileHeight));

    if(collisionLayers.empty()) {
        collisionLayers.push_back(0);
    }
}

void TA_Tilemap::draw(int priority)
{
    auto drawLayer = [&](int layer) {
        int lx = 0, rx = width - 1, ly = 0, ry = height - 1;
        if(camera != nullptr && TA::equal(position.x, 0) && TA::equal(position.y, 0)) {
            TA_Point cameraPos = camera->getPosition();
            lx = std::max(0, static_cast<int>(cameraPos.x / tileWidth));
            rx = std::min(width - 1, static_cast<int>((cameraPos.x + TA::screenWidth) / tileWidth));
            ly = std::max(0, static_cast<int>(cameraPos.y / tileWidth));
            ry = std::min(height - 1, static_cast<int>((cameraPos.y + TA::screenHeight) / tileWidth));
        }

        for(int tileX = lx; tileX <= rx; tileX ++) {
            for(int tileY = ly; tileY <= ry; tileY ++) {
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
        for(int layer = 0; layer < std::max(1, layerCount - 1); layer ++) {
            drawLayer(layer);
        }
    }
    else if(priority == 1 && layerCount >= 2) {
        drawLayer(layerCount - 1);
    }
}

void TA_Tilemap::setCamera(TA_Camera *newCamera)
{
    camera = newCamera;
    newCamera->setBorder({TA_Point(0, 0), TA_Point(width * tileWidth, height * tileHeight)});
    for(size_t tile = 0; tile < tileset.size(); tile += 1) {
        tileset[tile].sprite.setCamera(newCamera);
    }
}

void TA_Tilemap::setPosition(TA_Point position)
{
    this->position = position;
}

int TA_Tilemap::checkCollision(TA_Polygon &polygon, int halfSolidTop)
{
    int minX = 1e5, maxX = 0, minY = 1e5, maxY = 0;

    for(int pos = 0; pos < polygon.size(); pos ++) {
        TA_Point vertex = polygon.getVertex(pos);
        minX = std::min(minX, int(vertex.x / tileWidth));
        maxX = std::max(maxX, int(vertex.x / tileWidth));
        minY = std::min(minY, int(vertex.y / tileHeight));
        maxY = std::max(maxY, int(vertex.y / tileHeight));
    }

    auto normalize = [&](int &value, int left, int right) {
        value = std::max(value, left);
        value = std::min(value, right);
    };

    normalize(minX, 0, width - 1);
    normalize(maxX, 0, width - 1);
    normalize(minY, 0, height - 1);
    normalize(maxY, 0, height - 1);

    int flags = 0;

    auto checkCollisionWithTile = [&] (int layer, int tileX, int tileY)
    {
        int tileId = tilemap[layer][tileX][tileY];
        if(tileId == -1) {
            return;
        }

        for(auto& hitbox : tileset[tileId].hitboxes) {
            hitbox.polygon.setPosition(TA_Point(tileX * tileWidth, tileY * tileHeight));
            if((tileset[tileId].type != 1 || (tileY + 1) * tileHeight >= halfSolidTop)
                && polygon.intersects(hitbox.polygon)) {
                flags |= hitbox.type;
            }
        }
    };

    for(int layer : collisionLayers) {
        for(int tileX = minX; tileX <= maxX; tileX ++) {
            for(int tileY = minY; tileY <= maxY; tileY ++) {
                checkCollisionWithTile(layer, tileX, tileY);
            }
        }
    }

    for(int pos = 0; pos < (int)borderPolygons.size(); pos ++) {
        if(borderPolygons[pos].intersects(polygon)) {
            flags |= TA_COLLISION_SOLID;
        }
    }

    return flags;
}

void TA_Tilemap::setUpdateAnimation(bool enabled)
{
    updateAnimation = enabled;
}

std::vector<TA_Tilemap::Hitbox> TA_Tilemap::getSpikesHitboxVector(int type)
{
    std::vector<Hitbox> hitboxes;
    hitboxes.push_back(getSpikesSolidHitbox(type));
    hitboxes.push_back(getSpikesDamageHitbox(type));
    return hitboxes;
}

TA_Tilemap::Hitbox TA_Tilemap::getSpikesSolidHitbox(int type)
{
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

TA_Tilemap::Hitbox TA_Tilemap::getSpikesDamageHitbox(int type)
{
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
