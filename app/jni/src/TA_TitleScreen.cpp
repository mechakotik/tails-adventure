#include <algorithm>
#include <cmath>
#include "TA_TitleScreen.h"
#include "TA_Globals.h"
#include "TA_Tools.h"

void TA_TitleScreen::init()
{
    titleScreenSprite.load("title_screen/title_screen.png");
    titleScreenSprite.setPosition(gScreenWidth / 2 - 80, 0);
    touchToStartSprite.load("title_screen/touch_to_start.png");
    touchToStartSprite.setPosition(gScreenWidth / 2 - 57, 104);
    titleScreenSound.load("sound/title_screen.ogg", TA_SOUND_CHANNEL_MUSIC, 0);
    titleScreenSound.play();
}

TA_ScreenState TA_TitleScreen::update()
{
    localTimer += gElapsedTime;
    drawScreenRect(0, 0, 102, 255);
    titleScreenSprite.draw();

    auto drawTouchToStart = [&] (int delay, int fadeDelay)
    {
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

    drawTouchToStart(45, 5);
    if(localTimer < 10) {
        int factor = 255 - 255 * localTimer / 10;
        drawShadow(factor);
    }

    return TA_SCREENSTATE_CURRENT;
}

void TA_TitleScreen::quit()
{

}
