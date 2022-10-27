#ifndef TA_TILEMAP_H
#define TA_TILEMAP_H

#include <string>
#include <vector>
#include "sprite.h"
#include "camera.h"
#include "geometry.h"

class TA_Character;

class TA_Tilemap {
private:
    struct TA_Tile {
        TA_Polygon polygon;
        std::vector<int> animation;
        int animationDelay, type = 0;
    };

    struct TA_TilemapElement {
        TA_Sprite sprite;
        int tileIndex;
    };

    std::vector<std::vector<std::vector<TA_TilemapElement>>> tilemap;
    std::vector<TA_Tile> tileset;
    TA_Texture texture;
    int width, height, tileWidth, tileHeight, layerCount;

public:
    void load(std::string filename);
    void draw(int layer);
    void setCamera(TA_Camera *newCamera);
    int getWidth() {return width * tileWidth;}
    int getHeight() {return height * tileHeight;}
    void updateCollisions(TA_Polygon polygon, TA_Point vector, int layer, double &minVectorPos, int &flags);
};

#endif // TA_TILEMAP_H
