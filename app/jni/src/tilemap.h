#ifndef TA_TILEMAP_H
#define TA_TILEMAP_H

#include <string>
#include <vector>
#include <array>
#include "sprite.h"
#include "camera.h"
#include "geometry.h"

enum TA_CollisionType {
    TA_COLLISION_TRANSPARENT = 0,
    TA_COLLISION_SOLID = (1 << 0),
    TA_COLLISION_HALF_SOLID = (1 << 1),
    TA_COLLISION_DAMAGE = (1 << 2),
    TA_COLLISION_EXPLOSION = (1 << 3),
    TA_COLLISION_CHARACTER = (1 << 4),
    TA_COLLISION_RING = (1 << 5),
    TA_COLLISION_PUSHABLE = (1 << 6),
    TA_COLLISION_SPRING = (1 << 7),
    TA_COLLISION_BOMB = (1 << 8)
};

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
    std::array<TA_Polygon, 4> borderPolygons;
    TA_Texture texture;
    int width, height, tileWidth, tileHeight, layerCount, collisionLayer;

public:
    void load(std::string filename);
    void draw(int layer);
    void setCamera(TA_Camera *newCamera);
    int getWidth() {return width * tileWidth;}
    int getHeight() {return height * tileHeight;}
    void checkCollision(TA_Polygon &polygon, int &flags, int halfSolidTop = -1e9);
};

#endif // TA_TILEMAP_H
