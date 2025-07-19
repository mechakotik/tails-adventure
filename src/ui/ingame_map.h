#ifndef TA_INGAME_MAP_H
#define TA_INGAME_MAP_H

#include "font.h"
#include "sprite.h"

class TA_InGameMap {
private:
    TA_Sprite mapSprite;
    TA_Sprite namePlateSprite;
    TA_Font font;
    std::array<TA_Sprite, 2> dolphinSprites;
    std::array<TA_Sprite, 3> birdSprites;

    void drawBackground();

public:
    void load();
    void draw();
    void drawSelectionName(std::string name);
};

#endif // TA_INGAME_MAP_H
