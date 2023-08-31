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
    TA_COLLISION_PUSHABLE = (1 << 5),
    TA_COLLISION_SPRING = (1 << 6),
    TA_COLLISION_BOMB = (1 << 7),
    TA_COLLISION_EXPLOSION_FIRST = (1 << 8),
    TA_COLLISION_DAMAGE_X2 = (1 << 9),
    TA_COLLISION_MOVING_PLATFORM = (1 << 10),
    TA_COLLISION_INSTA_SHIELD = (1 << 11)
};

class TA_Tilemap {
private:
    struct Hitbox {
        TA_Polygon polygon;
        int type;
    };

    struct Tile {
        std::vector<Hitbox> hitboxes;
        std::vector<int> animation;
        int animationDelay, type = 0;
    };

    struct TilemapElement {
        TA_Sprite sprite;
        int tileIndex;
    };

    std::vector<Hitbox> getSpikesHitboxVector(int type);
    Hitbox getSpikesSolidHitbox(int type);
    Hitbox getSpikesDamageHitbox(int type);

    std::vector<std::vector<std::vector<TilemapElement>>> tilemap;
    std::vector<Tile> tileset;
    std::array<TA_Polygon, 3> borderPolygons;
    int width, height, tileWidth, tileHeight, layerCount, collisionLayer;

public:
    void load(std::string filename);
    void draw(int priority);
    void setCamera(TA_Camera *newCamera);
    int getWidth() {return width * tileWidth;}
    int getHeight() {return height * tileHeight;}
    int checkCollision(TA_Polygon &polygon, int halfSolidTop = -1e9);
    void setUpdateAnimation(bool enabled);
};

#endif // TA_TILEMAP_H
