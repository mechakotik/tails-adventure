#ifndef TA_HUD_H
#define TA_HUD_H

#include <array>
#include "sprite.h"
#include "links.h"
#include "sound.h"

class TA_Hud {
private:
    const double flightBarLeft = -9, flightBarRight = 3, flightBarY = 47;
    const double flightBarSpeed = 0.7;

    TA_Links links;
    TA_Sprite ringMonitor, itemSprite, flightBarSprite;
    std::array<TA_Sprite, 2> ringDigits;
    TA_Sound switchSound;
    int item = 0, itemPosition = 0;
    double flightBarX = flightBarLeft;

    void drawFlightBar();

public:
    void load(TA_Links newLinks);
    void update();
    void draw();
    int getCurrentItem() {return item;}
};

#endif // TA_HUD_H
