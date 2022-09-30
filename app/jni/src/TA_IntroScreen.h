#ifndef TA_INTROSCREEN_H
#define TA_INTROSCREEN_H

#include "TA_Screen.h"
#include "TA_Sprite.h"

class TA_IntroScreen : public TA_Screen {
private:
    int localTimer = 0;
    TA_Sprite segaLogoSprite, disclaimerSprite;
    bool secondAnimationPlayed = 0;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override;
};

#endif // TA_INTROSCREEN_H
