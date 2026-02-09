#ifndef TA_INGAME_MAP_H
#define TA_INGAME_MAP_H

#include <array>
#include "font.h"
#include "sprite.h"

class TA_InGameMap {
private:
    TA_Sprite mapSprite;
    TA_Sprite battleFortressSprite;
    TA_Sprite namePlateSprite;
    TA_Font font;
    std::array<TA_Sprite, 2> dolphinSprites;
    std::array<TA_Sprite, 3> birdSprites;

    bool battleFortressNeeded = false;
    TA_Point battleFortressBasePos;
    float timer = 0;

    void drawBackground();

public:
    void load();
    void draw();
    void drawSelectionName(std::string name);
};

#endif // TA_INGAME_MAP_H
