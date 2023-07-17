#ifndef TA_INGAME_MAP_H
#define TA_INGAME_MAP_H

#include "sprite.h"
#include "tilemap.h"
#include "font.h"

class TA_InGameMap {
private:
    TA_Tilemap tilemap;
    TA_Sprite namePlateSprite;
    TA_Font font;
    std::array<TA_Sprite, 2> dolphinSprites;

public:
    void load();
    void draw();
    void drawSelectionName(std::string name);
};

#endif // TA_INGAME_MAP_H
