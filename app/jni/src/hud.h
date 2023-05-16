#ifndef TA_HUD_H
#define TA_HUD_H

#include <array>
#include "sprite.h"
#include "links.h"

class TA_Hud {
private:
    TA_Links links;
    TA_Sprite ringMonitor, itemSprite;
    std::array<TA_Sprite, 2> ringDigits;
    int item = 0, itemPosition = 0;

public:
    void load(TA_Links newLinks);
    void update();
    void draw();
    int getCurrentItem() {return item;}
};

#endif // TA_HUD_H
