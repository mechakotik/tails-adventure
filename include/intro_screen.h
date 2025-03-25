#ifndef TA_INTROSCREEN_H
#define TA_INTROSCREEN_H

#include "screen.h"
#include "sprite.h"

class TA_IntroScreen : public TA_Screen {
private:
    float localTimer = 0;
    TA_Sprite segaLogoSprite, disclaimerSprite;
    bool secondAnimationPlayed = 0;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override;
};

#endif // TA_INTROSCREEN_H
