#include "intro_screen.h"
#include "engine/globals.h"
#include "engine/tools.h"

void TA_IntroScreen::init()
{
    segaLogoSprite.load("intro/sega_logo.png", 80, 25);
    segaLogoSprite.setPosition(gScreenWidth / 2 - 40, gScreenHeight / 2 - 12);
    segaLogoSprite.setAnimation(TA_Animation(0, 16, 3, 1));
    disclaimerSprite.load("intro/disclaimer.png");
    disclaimerSprite.setPosition(gScreenWidth / 2 - 128, 0);
    localTimer = 0;
}

TA_ScreenState TA_IntroScreen::update()
{
    localTimer += gElapsedTime;

    if(localTimer <= 130) {
        if(localTimer >= 70 && !secondAnimationPlayed) {
            segaLogoSprite.setAnimation(TA_Animation(17, 23, 2, 1));
            secondAnimationPlayed = 1;
        }
        segaLogoSprite.draw();

        if(localTimer >= 120) {
            int factor = 255 * (localTimer - 120) / 10;
            drawShadow(factor);
        }
    }
    else if(localTimer >= 150) {
        disclaimerSprite.draw();

        if(localTimer <= 160) {
            int factor = 255 - 255 * (localTimer - 150) / 10;
            drawShadow(factor);
        }
        if(localTimer >= 300) {
            int factor = 255 * (localTimer - 300) / 10;
            drawShadow(factor);
        }
    }

    if(localTimer >= 330) {
        return TA_SCREENSTATE_TITLE;
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_IntroScreen::quit()
{

}
