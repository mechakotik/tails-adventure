#ifndef TA_TITLESCREEN_H
#define TA_TITLESCREEN_H

#include "TA_Screen.h"
#include "TA_Sprite.h"
#include "TA_Sound.h"

class TA_TitleScreen : public TA_Screen {
private:
    double localTimer = 0;
    TA_Sprite titleScreenSprite, touchToStartSprite;
    TA_Sound titleScreenSound;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override;
};


#endif // TA_TITLESCREEN_H
