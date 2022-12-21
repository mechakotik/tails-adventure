#ifndef TA_HUD_H
#define TA_HUD_H

#include <array>
#include "engine/sprite.h"
#include "engine/links.h"

class TA_Hud {
private:
    TA_GameScreenLinks links;
    TA_Sprite ringMonitor, currentItem;
    std::array<TA_Sprite, 2> ringDigits;

public:
    void load(TA_GameScreenLinks newLinks);
    void draw();
};

#endif // TA_HUD_H
