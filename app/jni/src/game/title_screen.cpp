#include <algorithm>
#include <cmath>
#include "title_screen.h"
#include "engine/tools.h"
#include "engine/error.h"

void TA_TitleScreen::init()
{
    titleScreenSprite.load("title_screen/title_screen.png");
    titleScreenSprite.setPosition(TA::screenWidth / 2 - 80, 0);
    touchToStartSprite.load("title_screen/touch_to_start.png");
    touchToStartSprite.setPosition(TA::screenWidth / 2 - 57, 104);

    titleScreenSound.load("sound/title_screen.ogg", TA_SOUND_CHANNEL_MUSIC, 0);
    enterSound.load("sound/enter.ogg", TA_SOUND_CHANNEL_SFX, 0);
    titleScreenSound.play();

    button.setRectangle(TA_Point(0, 0), TA_Point(TA::screenWidth, TA::screenHeight));
}

TA_ScreenState TA_TitleScreen::update()
{
    localTimer += TA::elapsedTime;

    button.update();
    if(button.isJustPressed() && timePressed == -1) {
        enterSound.play();
        timePressed = localTimer;
    }

    TA::drawScreenRect(0, 0, 102, 255);
    titleScreenSprite.draw();

    auto drawTouchToStart = [&] (int delay, int fadeDelay) {
        int frame = int(localTimer) % (delay * 2), alpha;
        if(frame < delay) {
            alpha = 255 * std::max(0, frame - (delay - fadeDelay)) / fadeDelay;
        }
        else {
            alpha = 255 - 255 * std::max(0, frame - (delay * 2 - fadeDelay)) / fadeDelay;
        }
        touchToStartSprite.setAlpha(alpha);
        touchToStartSprite.draw();
    };

    if(timePressed == -1) {
        drawTouchToStart(30, 5);
    }
    else {
        drawTouchToStart(5, 2);
    }

    if(localTimer < 10) {
        int factor = 255 - 255 * localTimer / 10;
        TA::drawShadow(factor);
    }
    else if(timePressed != -1 && localTimer - timePressed > 60) {
        if(!startedFadeOut) {
            titleScreenSound.fadeOut(10);
            startedFadeOut = true;
        }
        int factor = 255 * (localTimer - timePressed - 60) / 10;
        TA::drawShadow(factor);
    }

    if(timePressed != -1 && localTimer - timePressed > 80) {
        return TA_SCREENSTATE_GAME;
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_TitleScreen::quit()
{

}
