#ifndef TA_TITLESCREEN_H
#define TA_TITLESCREEN_H

#include "screen.h"
#include "sprite.h"
#include "sound.h"
#include "touchscreen.h"

class TA_TitleScreen : public TA_Screen {
private:
    double localTimer = 0;
    int timePressed = -1;
    bool startedFadeOut = 0;
    TA_Sprite titleScreenSprite, touchToStartSprite;
    TA_Sound titleScreenSound,enterSound;
    TA_Button button;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override;
};


#endif // TA_TITLESCREEN_H
