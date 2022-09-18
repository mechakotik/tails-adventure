#include "TA_IntroScreen.h"
#include "TA_Globals.h"
#include "TA_Time.h"

void TA_IntroScreen::init(int param)
{
    segaLogoSprite.load("intro/sega_logo.png", 80, 25);
    segaLogoSprite.setPosition(gScreenWidth / 2 - 40, gScreenHeight / 2 - 12);
    segaLogoSprite.setAnimation(0, 16, convertFrames(3), 1);
}

TA_ScreenState TA_IntroScreen::update()
{
    localTimer = (localTimer + gElapsedTime);

    if(localTimer >= convertFrames(70) && !secondAnimationPlayed) {
        segaLogoSprite.setAnimation(17, 23, convertFrames(2), 1);
        secondAnimationPlayed = 1;
    }
    segaLogoSprite.draw();

    return {TA_SCREENSTATE_CURRENT, 0};
}

void TA_IntroScreen::quit()
{

}
