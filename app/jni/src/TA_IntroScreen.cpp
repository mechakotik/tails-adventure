#include "TA_IntroScreen.h"
#include "TA_Globals.h"
#include "TA_Tools.h"

void TA_IntroScreen::init()
{
    segaLogoSprite.load("intro/sega_logo.png", 80, 25);
    segaLogoSprite.setPosition(gScreenWidth / 2 - 40, gScreenHeight / 2 - 12);
    segaLogoSprite.setAnimation(0, 16, convertFrames(3), 1);
    disclaimerSprite.load("intro/disclaimer.png");
    disclaimerSprite.setPosition(gScreenWidth / 2 - 128, 0);
    localTimer = 0;
}

TA_ScreenState TA_IntroScreen::update()
{
    localTimer += gElapsedTime;

    if(localTimer <= convertFrames(130)) {
        if(localTimer >= convertFrames(70) && !secondAnimationPlayed) {
            segaLogoSprite.setAnimation(17, 23, convertFrames(2), 1);
            secondAnimationPlayed = 1;
        }
        segaLogoSprite.draw();

        if(localTimer >= convertFrames(120)) {
            int factor = 255 * (localTimer - convertFrames(120)) / convertFrames(10);
            drawShadow(factor);
        }
    }
    else if(localTimer >= convertFrames(150)) {
        disclaimerSprite.draw();

        if(localTimer <= convertFrames(160)) {
            int factor = 255 - 255 * (localTimer - convertFrames(150)) / convertFrames(10);
            drawShadow(factor);
        }
        if(localTimer >= convertFrames(300)) {
            int factor = 255 * (localTimer - convertFrames(300)) / convertFrames(10);
            drawShadow(factor);
        }
    }

    if(localTimer >= convertFrames(330)) {
        return TA_SCREENSTATE_TITLE;
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_IntroScreen::quit()
{

}
