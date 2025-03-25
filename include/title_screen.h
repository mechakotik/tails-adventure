#ifndef TA_TITLE_SCREEN_H
#define TA_TITLE_SCREEN_H

#include "controller.h"
#include "screen.h"
#include "sound.h"
#include "sprite.h"
#include "touchscreen.h"

class TA_TitleScreen : public TA_Screen {
private:
    enum State { STATE_PRESS_START, STATE_HIDE_PRESS_START, STATE_EXIT };

    void updatePressStart();
    void updateHidePressStart();
    void updateExit();

    State state = STATE_PRESS_START;
    float timer = 0, alpha = 0;
    bool shouldExit = false;

    TA_Sprite backgroundSprite, pressStartSprite;
    TA_Sound enterSound;
    TA_Controller controller;
    TA_OnscreenButton button;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override;
};

#endif // TA_TITLE_SCREEN_H
