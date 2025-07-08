#ifndef TA_GAME_OVER_SCREEN_H
#define TA_GAME_OVER_SCREEN_H

#include "controller.h"
#include "screen_state_machine.h"
#include "sprite.h"

class TA_GameOverScreen : public TA_Screen {
private:
    TA_Controller controller;
    TA_Sprite gameOverSprite;
    TA_OnscreenButton button;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override {}
};

#endif // TA_GAME_OVER_SCREEN_H
