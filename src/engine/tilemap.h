#ifndef TA_TILEMAP_H
#define TA_TILEMAP_H

#include <array>
#include <string>
#include <tmxpp.hpp>
#include <vector>
#include "camera.h"
#include "geometry.h"
#include "sprite.h"

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
    std::vector<int> normalLayers;
    std::vector<int> priorityLayers;
    std::vector<int> layerAlpha;
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
    void setLayerAlpha(int layer, int alpha);
    void updateBorders();
    int getWidth() { return width * tileWidth; }
    int getHeight() { return height * tileHeight; }
    int getNumLayers() { return static_cast<int>(tilemap.size()); }
    int checkCollision(TA_Rect& rect);
    void setUpdateAnimation(bool enabled);
};

#endif // TA_TILEMAP_H
