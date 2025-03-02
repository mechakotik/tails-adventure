#include "game_over_screen.h"
#include "save.h"
#include "sound.h"
#include "tools.h"

void TA_GameOverScreen::init() {
    gameOverSprite.load("gameover/gameover.png");
    controller.load();
    TA::sound::playMusic("sound/game_over.vgm", 0);
    setMaxRings();
    button.setRectangle(TA_Point(0, 0), TA_Point(TA::screenWidth, TA::screenHeight));

    TA::save::setSaveParameter("map_position", 0);
    TA::save::setSaveParameter("seafox", false);
}

void TA_GameOverScreen::setMaxRings() {
    long long itemMask = TA::save::getSaveParameter("item_mask");
    int rings = 8;
    for(int num = 29; num <= 34; num++) {
        if(itemMask & (1ll << num)) {
            rings += 2;
        }
    }
    TA::save::setSaveParameter("rings", rings);
}

TA_ScreenState TA_GameOverScreen::update() {
    TA::drawScreenRect(17, 17, 153, 255);
    gameOverSprite.setPosition(TA::screenWidth / 2 - gameOverSprite.getWidth() / 2, 0);
    gameOverSprite.draw();

    controller.update();
    button.update();
    if(controller.isJustPressed(TA_BUTTON_PAUSE) || button.isJustPressed() || !TA::sound::isMusicPlaying()) {
        return TA_SCREENSTATE_TITLE;
    }
    return TA_SCREENSTATE_CURRENT;
}
