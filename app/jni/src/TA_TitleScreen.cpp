#include <algorithm>
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

    if(localTimer <= convertFrames(10)) {
        int factor = 255 - 255 * localTimer / convertFrames(10);
        drawShadow(factor);
    }
    else if(localTimer >= convertFrames(15)) {
        int frame = ((localTimer - convertFrames(15)) / convertFrames(1)) % 90;
        if(frame < 45) {
            int alpha = 255 - 255 * std::max(0, frame - 40) / 5;
            touchToStartSprite.setAlpha(alpha);
        }
        else {
            int alpha = 255 * std::max(0, frame - 85) / 5;
            touchToStartSprite.setAlpha(alpha);
        }
        touchToStartSprite.draw();
    }

    return TA_SCREENSTATE_CURRENT;
}

void TA_TitleScreen::quit()
{

}
