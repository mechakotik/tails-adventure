#ifndef TA_HUD_H
#define TA_HUD_H

#include <array>
#include "sprite.h"
#include "links.h"
#include "sound.h"
#include "font.h"
#include "screen_state_machine.h"
#include "touchscreen.h"

class TA_Hud {
private:
    const double flightBarLeft = -9, flightBarRight = 3, flightBarY = 47;
    const double flightBarSpeed = 0.7;
    const double ringAddTime = 6, fadeTime = 6;

    TA_Links links;
    TA_Sprite ringMonitor, itemSprite, flightBarSprite, leftSprite, rightSprite, pauseSprite;
    TA_OnscreenButton leftButton, rightButton, pauseButton;
    std::array<TA_Sprite, 2> ringDigits;
    TA_Sound switchSound, itemSwitchSound, pauseSound;
    int item = 0, itemPosition = 0, rings;
    double flightBarX = flightBarLeft;
    double timer = 0;

    TA_ScreenState transition = TA_SCREENSTATE_CURRENT;
    TA_Sprite pauseMenuItemSprite, pointerSprite, pauseMenuFrameSprite;
    TA_Font pauseMenuFont, pauseMenuInactiveFont;
    int pauseMenuSelection = 0;
    double pauseMenuAlpha = 0;
    bool paused = false, exitPause = false;

    void updateRingsCounter();
    void updateCurrentItem();
    void updatePause();
    void updatePauseMenu();
    void pauseMenuSelect();

    void setHudAlpha(int alpha);
    void setPauseMenuAlpha(int alpha);

    void drawFlightBar();
    void drawCurrentItem();
    void drawTouchControls();
    void drawRingsCounter();
    void drawPauseMenu();

public:
    void load(TA_Links newLinks);
    void update();
    void draw();
    int getCurrentItem() {return item;}
    bool isPaused() {return paused;}
    TA_ScreenState getTransition() {return transition;}
};

#endif // TA_HUD_H
