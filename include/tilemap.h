#ifndef TA_TILEMAP_H
#define TA_TILEMAP_H

#include <array>
#include <string>
#include <tmxpp.hpp>
#include <vector>
#include "camera.h"
#include "geometry.h"
#include "sprite.h"

enum TA_CollisionType {
    TA_COLLISION_TRANSPARENT = 0,
    TA_COLLISION_SOLID = (1 << 0),
    TA_COLLISION_SOLID_UP = (1 << 1),
    TA_COLLISION_SOLID_DOWN = (1 << 2),
    TA_COLLISION_DAMAGE = (1 << 3),
    TA_COLLISION_ATTACK = (1 << 4),
    TA_COLLISION_BOMB = (1 << 5),
    TA_COLLISION_CHARACTER = (1 << 6),
    TA_COLLISION_PUSHABLE = (1 << 7),
    TA_COLLISION_SPRING = (1 << 8),
    TA_COLLISION_MOVING_PLATFORM = (1 << 9),
    TA_COLLISION_WATER = (1 << 10),
    TA_COLLISION_NAPALM = (1 << 11),
    TA_COLLISION_DRILL = (1 << 12),
    TA_COLLISION_CONVEYOR_BELT_LEFT = (1 << 13),
    TA_COLLISION_CONVEYOR_BELT_RIGHT = (1 << 14),
    TA_COLLISION_TARGET = (1 << 15)
};

class TA_Tilemap {
private:
    struct Hitbox {
        TA_Polygon polygon;
        int type;
    };

    struct Tile {
        TA_Sprite sprite;
        std::vector<Hitbox> hitboxes;
        int type = 0;
    };

    void loadTileset(const tmx::Tileset& tiles);
    void loadLayer(int id, const tmx::Layer& layer);

    std::vector<Hitbox> getSpikesHitboxVector(int type);
    Hitbox getSpikesSolidHitbox(int type);
    Hitbox getSpikesDamageHitbox(int type);

    std::vector<std::vector<std::vector<int>>> tilemap;
    std::vector<Tile> tileset;
    std::array<TA_Polygon, 4> borderPolygons;
    std::vector<int> collisionLayers;
    std::filesystem::path filename;
    TA_Camera* camera = nullptr;
    TA_Point position;
    int width, height, tileWidth, tileHeight, layerCount;
    int borderMask = 13;
    bool updateAnimation = true;

public:
    void load(std::string filename);
    void draw(int priority);
    void setCamera(TA_Camera* newCamera);
    void setPosition(TA_Point position);
    void setBorderMask(int mask) { borderMask = mask; }
    void updateBorders();
    int getWidth() { return width * tileWidth; }
    int getHeight() { return height * tileHeight; }
    int checkCollision(TA_Rect& rect);
    void setUpdateAnimation(bool enabled);
};

#endif // TA_TILEMAP_H
