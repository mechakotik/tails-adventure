#include <algorithm>
#include <cmath>
#include "title_screen.h"
#include "tools.h"
#include "error.h"
#include "save.h"

void TA_TitleScreen::init()
{
    backgroundSprite.load("title_screen/title_screen.png");
    backgroundSprite.setPosition(TA::screenWidth / 2 - 80, 0);
    pressStartSprite.load("title_screen/press_start.png");
    pressStartSprite.setPosition(TA::screenWidth / 2 - pressStartSprite.getWidth() / 2, 104);

    menuSprite.load("title_screen/menu.png");
    menuSprite.setPosition(TA::screenWidth / 2 - menuSprite.getWidth() / 2, 96);
    selectorSprite.load("title_screen/selector.png");
    selectorSprite.setPosition(TA::screenWidth / 2 - selectorSprite.getWidth() / 2, 112);

    titleScreenSound.load("sound/title_screen.ogg", TA_SOUND_CHANNEL_MUSIC, 0);
    enterSound.load("sound/enter.ogg", TA_SOUND_CHANNEL_SFX1, 0);
    switchSound.load("sound/switch.ogg", TA_SOUND_CHANNEL_SFX2, 0);
    titleScreenSound.play();
    controller.load();
}

TA_ScreenState TA_TitleScreen::update()
{
    TA::drawScreenRect(0, 0, 102, 255);
    backgroundSprite.draw();
    controller.update();

    switch(state) {
        case STATE_PRESS_START:
            updatePressStart();
            break;
        case STATE_HIDE_PRESS_START:
            updateHidePressStart();
            break;
        case STATE_SHOW_MENU:
            updateShowMenu();
            break;
        case STATE_MENU:
            updateMenu();
            break;
        case STATE_EXIT:
            updateExit();
            break;
        default:
            break;
    }

    if(shouldExit) {
        return TA_SCREENSTATE_MAP;
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_TitleScreen::updatePressStart()
{
    const double idleTime = 30;
    const double transitionTime = 5;

    timer += TA::elapsedTime;
    timer = std::fmod(timer, (idleTime + transitionTime) * 2);

    if(timer < transitionTime) {
        menuAlpha = 255 * timer / transitionTime;
    }
    else if(timer < transitionTime + idleTime) {
        menuAlpha = 255;
    }
    else if(timer < transitionTime * 2 + idleTime) {
        menuAlpha = 255 - 255 * (timer - (idleTime + transitionTime)) / transitionTime;
    }
    else {
        menuAlpha = 0;
    }

    pressStartSprite.setAlpha(menuAlpha);
    pressStartSprite.draw();

    if(controller.isJustPressed(TA_BUTTON_PAUSE)) {
        state = STATE_HIDE_PRESS_START;
        enterSound.play();
    }
}

void TA_TitleScreen::updateHidePressStart()
{
    const double disappearTime = 5;
    menuAlpha -= 255 * (disappearTime / TA::elapsedTime);

    pressStartSprite.setAlpha(menuAlpha);
    pressStartSprite.draw();

    if(menuAlpha < 0) {
        state = STATE_SHOW_MENU;
        menuAlpha = 0;
    }
}

void TA_TitleScreen::updateShowMenu()
{
    const double showTime = 5;
    menuAlpha += 255 * TA::elapsedTime / showTime;

    menuSprite.setAlpha(menuAlpha);
    selectorSprite.setAlpha(menuAlpha);

    menuSprite.draw();
    selectorSprite.draw();

    if(menuAlpha >= 255) {
        state = STATE_MENU;
        menuAlpha = 255;
    }
}

void TA_TitleScreen::updateMenu()
{
    if(controller.isJustChangedDirection()) {
        TA_Direction direction = controller.getDirection();
        if(direction == TA_DIRECTION_UP && selection == 1) {
            selection = 0;
            switchSound.play();
        }
        else if(direction == TA_DIRECTION_DOWN && selection == 0) {
            selection = 1;
            switchSound.play();
        }
    }

    if(controller.isJustPressed(TA_BUTTON_A) || controller.isJustPressed(TA_BUTTON_B) || controller.isJustPressed(TA_BUTTON_PAUSE)) {
        if(selection == 0) {
            TA::save::createSave("save_0");
        }
        else {
            TA::save::repairSave("save_0");
        }
        TA::save::setCurrentSave("save_0");
        enterSound.play();
        state = STATE_EXIT;
        timer = 0;
    }

    selectorSprite.setPosition(TA::screenWidth / 2 - selectorSprite.getWidth() / 2, 96 + 16 * selection);
    menuSprite.draw();
    selectorSprite.draw();
}

void TA_TitleScreen::updateExit()
{
    const double exitTime = 20;

    timer += TA::elapsedTime;
    if(timer > exitTime) {
        shouldExit = true;
    }

    menuSprite.draw();
    selectorSprite.draw();
}

void TA_TitleScreen::quit()
{

}
