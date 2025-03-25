#include "title_screen.h"
#include <algorithm>
#include <cmath>
#include "error.h"
#include "save.h"
#include "tools.h"

void TA_TitleScreen::init() {
    backgroundSprite.load("title_screen/title_screen.png");

#ifdef __ANDROID__
    pressStartSprite.load("title_screen/touch_to_start.png");
#else
    pressStartSprite.load("title_screen/press_start.png");
#endif

    TA::sound::playMusic("sound/title.vgm");
    enterSound.load("sound/enter.ogg", TA_SOUND_CHANNEL_SFX1, 0);

    controller.load();
    button.setRectangle(TA_Point(0, 0), TA_Point(TA::screenWidth, TA::screenHeight));
}

TA_ScreenState TA_TitleScreen::update() {
    backgroundSprite.setPosition(
        (TA::screenWidth - backgroundSprite.getWidth()) / 2, (TA::screenHeight - backgroundSprite.getHeight()) / 2);
    pressStartSprite.setPosition(TA::screenWidth / 2 - pressStartSprite.getWidth() / 2,
        104 + (TA::screenHeight - backgroundSprite.getHeight()) / 2);

    TA::drawScreenRect(0, 0, 102, 255);
    backgroundSprite.draw();
    controller.update();
    button.update();

    switch(state) {
        case STATE_PRESS_START:
            updatePressStart();
            break;
        case STATE_HIDE_PRESS_START:
            updateHidePressStart();
            break;
        case STATE_EXIT:
            updateExit();
            break;
        default:
            break;
    }

    if(shouldExit) {
        return TA_SCREENSTATE_MAIN_MENU;
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_TitleScreen::updatePressStart() {
    const double idleTime = 30;
    const double transitionTime = 5;

    timer += TA::elapsedTime;
    timer = std::fmod(timer, (idleTime + transitionTime) * 2);

    if(timer < transitionTime) {
        alpha = 255 * timer / transitionTime;
    } else if(timer < transitionTime + idleTime) {
        alpha = 255;
    } else if(timer < transitionTime * 2 + idleTime) {
        alpha = 255 - 255 * (timer - (idleTime + transitionTime)) / transitionTime;
    } else {
        alpha = 0;
    }

    pressStartSprite.setAlpha(alpha);
    pressStartSprite.draw();

    if(controller.isJustPressed(TA_BUTTON_PAUSE) || button.isPressed()) {
        state = STATE_HIDE_PRESS_START;
        enterSound.play();
    }
}

void TA_TitleScreen::updateHidePressStart() {
    const double disappearTime = 5;
    alpha += 255 * (disappearTime / TA::elapsedTime);

    pressStartSprite.setAlpha(alpha);
    pressStartSprite.draw();

    if(alpha >= 255) {
        state = STATE_EXIT;
        timer = 0;
        alpha = 255;
    }
}

void TA_TitleScreen::updateExit() {
    const double exitTime = 14;
    pressStartSprite.draw();

    timer += TA::elapsedTime;
    if(timer > exitTime) {
        shouldExit = true;
    }
}

void TA_TitleScreen::quit() {}
