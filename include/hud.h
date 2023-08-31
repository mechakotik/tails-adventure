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
    const double ringAddTime = 6;

    struct PauseMenu {
        TA_Sprite sprite;
        double position;
    } pauseMenu;

    TA_Links links;
    TA_Sprite ringMonitor, itemSprite, flightBarSprite;
    std::array<TA_Sprite, 2> ringDigits;
    TA_Sound switchSound;
    int item = 0, itemPosition = 0, rings;
    double flightBarX = flightBarLeft;
    double timer = 0;
    bool paused = false;

    void updateRingsCounter();
    void updateCurrentItem();
    void updatePause();
    void updatePauseMenu();

    void drawFlightBar();
    void drawCurrentItem();
    void drawRingsCounter();
    void drawPauseMenu();

public:
    void load(TA_Links newLinks);
    void update();
    void draw();
    int getCurrentItem() {return item;}
    bool isPaused() {return paused;}
};

#endif // TA_HUD_H
