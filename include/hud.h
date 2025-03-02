#ifndef TA_HUD_H
#define TA_HUD_H

#include <array>
#include "links.h"
#include "pause_menu.h"
#include "screen.h"
#include "sound.h"
#include "sprite.h"
#include "touchscreen.h"

class TA_Hud {
private:
    static constexpr double flightBarLeft = -9, flightBarRight = 3, flightBarY = 47;
    static constexpr double flightBarSpeed = 0.7;
    static constexpr double ringAddTime = 6, fadeTime = 6;

    TA_Links links;
    TA_Sprite ringMonitor, itemSprite, flightBarSprite, leftSprite, rightSprite, pauseSprite;
    TA_OnscreenButton leftButton, rightButton, pauseButton;
    std::array<TA_Sprite, 2> ringDigits;
    TA_Sound switchSound, itemSwitchSound, pauseSound;
    int item = 0, itemPosition = 0, rings = 0;
    double flightBarX = flightBarLeft;
    double timer = 0;

    TA_ScreenState transition = TA_SCREENSTATE_CURRENT;
    TA_PauseMenu pauseMenu;
    bool paused = false, exitPause = false;

    void updateRingsCounter();
    void updateCurrentItem();
    void updatePause();
    void updatePauseMenu();
    void updatePauseMenuInputController();
    void updatePauseMenuInputTouch();
    void pauseMenuSelect();

    void setHudAlpha(int alpha);
    void drawFlightBar();
    void drawCurrentItem();
    void drawTouchControls();
    void drawRingsCounter();

public:
    void load(TA_Links newLinks);
    void update();
    void draw();
    [[nodiscard]] int getCurrentItem() const { return item; }
    [[nodiscard]] bool isPaused() const { return paused; }
    [[nodiscard]] TA_ScreenState getTransition() const { return transition; }
};

#endif // TA_HUD_H
