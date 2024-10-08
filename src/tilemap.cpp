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

    for(int pos = 0; pos < tileCount; pos ++) {
        tileset[pos].animation = std::vector<int>{pos};
        tileset[pos].animationDelay = 1;
    }

    std::string textureFilename = "";

    auto loadTileset = [&](tinyxml2::XMLElement *tilesetElement)
    {
        textureFilename = filename;
        while(!textureFilename.empty() && textureFilename.back() != '/') {
            textureFilename.pop_back();
        }
        textureFilename += tilesetElement->FirstChildElement("image")->Attribute("source");

        for(tinyxml2::XMLElement *tileElement = tilesetElement->FirstChildElement("tile");
            tileElement != nullptr; tileElement = tileElement->NextSiblingElement("tile"))
        {
            int tileId = tileElement->IntAttribute("id");
            if(tileElement->FirstChildElement("animation") != nullptr) {
                tileset[tileId].animation.clear();
                int delayMs = tileElement->FirstChildElement("animation")->FirstChildElement("frame")->IntAttribute("duration");
                tileset[tileId].animationDelay = int(delayMs * 60 / 1000 + 0.5);
                for(tinyxml2::XMLElement *frameElement = tileElement->FirstChildElement("animation")->FirstChildElement("frame");
                    frameElement != nullptr; frameElement = frameElement->NextSiblingElement("frame")) {
                    tileset[tileId].animation.push_back(frameElement->IntAttribute("tileid"));
                }
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
                tile --;
                tilemap[layer][tileX][tileY].tileIndex = tile;
                if(tile == -1) {
                    continue;
                }
                tilemap[layer][tileX][tileY].sprite.load(textureFilename, tileWidth, tileHeight);
                tilemap[layer][tileX][tileY].sprite.setPosition(tileX * tileWidth, tileY * tileHeight);
                tilemap[layer][tileX][tileY].sprite.setAnimation(TA_Animation(tileset[tile].animation, tileset[tile].animationDelay, -1));
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
        for(int tileX = 0; tileX < width; tileX ++) {
            for(int tileY = 0; tileY < height; tileY ++) {
                if(tilemap[layer][tileX][tileY].tileIndex != -1) {
                    tilemap[layer][tileX][tileY].sprite.draw();
                }
            }
        }
    };

    if(priority == 0) {
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
    newCamera->setBorder({TA_Point(0, 0), TA_Point(width * tileWidth, height * tileHeight)});
    for(int layer = 0; layer < layerCount; layer ++) {
        for(int tileX = 0; tileX < width; tileX ++) {
            for(int tileY = 0; tileY < height; tileY ++) {
                if(tilemap[layer][tileX][tileY].tileIndex != -1) {
                    tilemap[layer][tileX][tileY].sprite.setCamera(newCamera);
                }
            }
        }
    }
}

void TA_Tilemap::setPosition(TA_Point position)
{
    for(int layer = 0; layer < layerCount; layer ++) {
        for(int tileX = 0; tileX < width; tileX ++) {
            for(int tileY = 0; tileY < height; tileY ++) {
                if(tilemap[layer][tileX][tileY].tileIndex != -1) {
                    TA_Point tilePosition{position.x + tileX * tileWidth, position.y + tileY * tileHeight};
                    tilemap[layer][tileX][tileY].sprite.setPosition(tilePosition);
                }
            }
        }
    }
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
        int tileId = tilemap[layer][tileX][tileY].tileIndex;
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
    for(int layer = 0; layer < layerCount; layer ++) {
        for(int tileX = 0; tileX < width; tileX ++) {
            for(int tileY = 0; tileY < height; tileY ++) {
                if(tilemap[layer][tileX][tileY].tileIndex != -1) {
                    tilemap[layer][tileX][tileY].sprite.setUpdateAnimation(enabled);
                }
            }
        }
    }
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
