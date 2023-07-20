#ifndef TA_GAME_OVER_SCREEN_H
#define TA_GAME_OVER_SCREEN_H

#include "screen_state_machine.h"
#include "sprite.h"
#include "controller.h"

class TA_GameOverScreen : public TA_Screen {
private:
    void setMaxRings();

    TA_CommonController controller;
    TA_Sprite gameOverSprite;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override {}
};

#endif // TA_GAME_OVER_SCREEN_H
