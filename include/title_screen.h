#ifndef TA_TITLESCREEN_H
#define TA_TITLESCREEN_H

#include "screen.h"
#include "sprite.h"
#include "sound.h"
#include "touchscreen.h"
#include "controller.h"

class TA_TitleScreen : public TA_Screen {
private:
    double localTimer = 0;
    int timePressed = -1;
    bool startedFadeOut = false, timeOut = false;
    TA_Sprite titleScreenSprite, pressStartSprite;
    TA_Sound titleScreenSound,enterSound;
    TA_Button button;
    TA_Controller controller;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override;
};


#endif // TA_TITLESCREEN_H
