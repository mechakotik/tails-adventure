#include <sstream>
#include <SDL.h>
#include "tilemap.h"
#include "tinyxml2.h"
#include "error.h"
#include "tools.h"
#include "globals.h"

void TA_Tilemap::load(std::string filename)
{
    tinyxml2::XMLDocument xmlFile;
    xmlFile.Parse(readStringFromFile(filename).c_str());
    tinyxml2::XMLElement *xmlRoot = xmlFile.FirstChildElement("map");

    width = xmlRoot->IntAttribute("width");
    height = xmlRoot->IntAttribute("height");
    tileWidth = xmlRoot->FirstChildElement("tileset")->IntAttribute("tilewidth");
    tileHeight = xmlRoot->FirstChildElement("tileset")->IntAttribute("tileheight");
    int tileCount = xmlRoot->FirstChildElement("tileset")->IntAttribute("tilecount");

    tilemap.resize(width);
    for(int pos = 0; pos < width; pos ++) {
        tilemap[pos].resize(height);
    }
    tileset.assign(tileCount, TA_Tile());

    for(int pos = 0; pos < tileCount; pos ++) {
        tileset[pos].priority = 0;
        tileset[pos].animation = std::vector<int>{pos};
        tileset[pos].animationDelay = 1;
    }

    std::string textureFilename = filename;
    while(!textureFilename.empty() && textureFilename.back() != '/') {
        textureFilename.pop_back();
    }
    textureFilename += xmlRoot->FirstChildElement("tileset")->FirstChildElement("image")->Attribute("source");
    texture.load(textureFilename);

    std::stringstream mapStream;
    mapStream << xmlRoot->FirstChildElement("layer")->FirstChildElement("data")->GetText();

    for(int tileY = 0; tileY < height; tileY ++ ) {
        for(int tileX = 0; tileX < width; tileX ++) {
            int tile;
            char temp;
            mapStream >> tile;
            if(tileX != width - 1 || tileY != height - 1) {
                mapStream >> temp;
            }
            tile --;
            tilemap[tileX][tileY].tileIndex = tile;
            tilemap[tileX][tileY].sprite.loadFromTexture(&texture, tileWidth, tileHeight);
            tilemap[tileX][tileY].sprite.setPosition(tileX * tileWidth, tileY * tileHeight);
            tilemap[tileX][tileY].sprite.setAnimation(tileset[tile].animation, tileset[tile].animationDelay, -1);
        }
    }
}

void TA_Tilemap::draw(int priority)
{
    for(int tileX = 0; tileX < width; tileX ++) {
        for(int tileY = 0; tileY < height; tileY ++) {
            if(tileset[tilemap[tileX][tileY].tileIndex].priority == priority) {
                tilemap[tileX][tileY].sprite.draw();
            }
        }
    }
}

void TA_Tilemap::setCamera(TA_Camera *newCamera)
{
    newCamera->setBorder({TA_Point(0, 0), TA_Point(width * tileWidth - gScreenWidth, height * tileHeight - gScreenHeight)});
    for(int tileX = 0; tileX < width; tileX ++) {
        for(int tileY = 0; tileY < height; tileY ++) {
            tilemap[tileX][tileY].sprite.setCamera(newCamera);
        }
    }
}
