#ifndef TA_TILEMAP_H
#define TA_TILEMAP_H

#include <string>
#include <vector>
#include "sprite.h"
#include "camera.h"

class TA_Tilemap {
private:
    struct TA_Tile {
        std::vector<int> animation;
        int priority, animationDelay;
    };

    struct TA_TilemapElement {
        TA_Sprite sprite;
        int tileIndex;
    };

    std::vector<std::vector<TA_TilemapElement>> tilemap;
    std::vector<TA_Tile> tileset;
    TA_Texture texture;
    int width, height, tileWidth, tileHeight;

public:
    void load(std::string filename);
    void draw(int priority);
    void setCamera(TA_Camera *newCamera);
};

#endif // TA_TILEMAP_H
