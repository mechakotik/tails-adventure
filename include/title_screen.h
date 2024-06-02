#ifndef TA_TITLE_SCREEN_H
#define TA_TITLE_SCREEN_H

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
        STATE_EXIT
    };

    void updatePressStart();
    void updateHidePressStart();
    void updateExit();

    State state = STATE_PRESS_START;
    double timer = 0, alpha = 0;
    bool shouldExit = false;

    TA_Sprite backgroundSprite, pressStartSprite;
    TA_Sound titleScreenSound, enterSound;
    TA_Controller controller;
    TA_OnscreenButton button;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override;
};


#endif // TA_TITLE_SCREEN_H
