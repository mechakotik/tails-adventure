#include <algorithm>
#include <cmath>
#include "title_screen.h"
#include "tools.h"
#include "error.h"

void TA_TitleScreen::init()
{
    titleScreenSprite.load("title_screen/title_screen.png");
    titleScreenSprite.setPosition(TA::screenWidth / 2 - 80, 0);
    pressStartSprite.load("title_screen/press_start.png");
    pressStartSprite.setPosition(TA::screenWidth / 2 - pressStartSprite.getWidth() / 2, 104);

    titleScreenSound.load("sound/title_screen.ogg", TA_SOUND_CHANNEL_MUSIC, 0);
    enterSound.load("sound/enter.ogg", TA_SOUND_CHANNEL_SFX1, 0);
    titleScreenSound.play();

    button.setRectangle(TA_Point(0, 0), TA_Point(TA::screenWidth, TA::screenHeight));
    controller.load();
}

TA_ScreenState TA_TitleScreen::update()
{
    localTimer += TA::elapsedTime;

    button.update();
    controller.update();

    if(timePressed == -1) {
        if(controller.isJustPressed(TA_BUTTON_PAUSE) || button.isJustPressed()) {
            enterSound.play();
            timePressed = localTimer;
        }
        else if(!TA::sound::isPlaying(TA_SOUND_CHANNEL_MUSIC)) {
            timeOut = true;
            timePressed = localTimer;
        }
    }

    TA::drawScreenRect(0, 0, 102, 255);
    titleScreenSprite.draw();

    auto drawPressStart = [&] (int delay, int fadeDelay) {
        int frame = int(localTimer) % (delay * 2), alpha;
        if(frame < delay) {
            alpha = 255 * std::max(0, frame - (delay - fadeDelay)) / fadeDelay;
        }
        else {
            alpha = 255 - 255 * std::max(0, frame - (delay * 2 - fadeDelay)) / fadeDelay;
        }
        pressStartSprite.setAlpha(alpha);
        pressStartSprite.draw();
    };

    if(timePressed == -1 || timeOut) {
        drawPressStart(30, 5);
    }
    else {
        drawPressStart(5, 2);
    }

    if(localTimer < 10) {
        int factor = 255 - 255 * localTimer / 10;
        TA::drawShadow(factor);
    }
    else if(timePressed != -1 && localTimer - timePressed > 40) {
        if(!startedFadeOut) {
            titleScreenSound.fadeOut(10);
            startedFadeOut = true;
        }
        int factor = 255 * (localTimer - timePressed - 40) / 10;
        TA::drawShadow(factor);
    }

    if(timePressed != -1 && localTimer - timePressed > 50) {
        return TA_SCREENSTATE_MAP;
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_TitleScreen::quit()
{

}
