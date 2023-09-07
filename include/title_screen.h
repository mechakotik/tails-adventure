#ifndef TA_TITLESCREEN_H
#define TA_TITLESCREEN_H

#include "screen.h"
#include "sprite.h"
#include "sound.h"
#include "touchscreen.h"
#include "controller.h"

class TA_TitleScreen : public TA_Screen {
private:
    enum State {
        STATE_PRESS_START,
        STATE_HIDE_PRESS_START,
        STATE_SHOW_MENU,
        STATE_MENU,
        STATE_EXIT
    };

    void updatePressStart();
    void updateHidePressStart();
    void updateShowMenu();
    void updateMenu();
    void updateExit();

    State state = STATE_PRESS_START;
    double timer = 0, menuAlpha = 0;
    int selection = 1;
    bool shouldExit = false;

    TA_Sprite backgroundSprite, pressStartSprite, menuSprite, selectorSprite;
    TA_Sound titleScreenSound, enterSound, switchSound;
    TA_Controller controller;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override;
};


#endif // TA_TITLESCREEN_H
