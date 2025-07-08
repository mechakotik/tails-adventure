#include "main_menu_screen.h"
#include "data_select_section.h"
#include "options_section.h"
#include "sound.h"

void TA_MainMenuScreen::init() {
    TA::sound::playMusic("sound/password.vgm");
    titleSprite.load("data_select/title.png", 112, 9);
    controller.load();

    sections[TA_MAIN_MENU_DATA_SELECT] = std::make_unique<TA_DataSelectSection>(TA_DataSelectSection(&controller));
    sections[TA_MAIN_MENU_DATA_SELECT]->load();

    sections[TA_MAIN_MENU_OPTIONS] = std::make_unique<TA_OptionsSection>(TA_OptionsSection(&controller));
    sections[TA_MAIN_MENU_OPTIONS]->load();

    state = neededState = TA_MAIN_MENU_DATA_SELECT;
}

TA_ScreenState TA_MainMenuScreen::update() {
    controller.update();
    updateTitle();

    if(state == neededState) {
        neededState = sections[state]->update();
        sections[state]->draw();
    } else {
        if(neededState == TA_MAIN_MENU_EXIT) {
            sections[state]->draw();
            return TA_SCREENSTATE_MAP;
        } else {
            timer += TA::elapsedTime;
            if(timer < transitionTime) {
                sections[state]->setAlpha(255 - 255 * timer / transitionTime);
                sections[state]->draw();
            } else if(timer < transitionTime * 2) {
                sections[neededState]->setAlpha(255 * (timer - transitionTime) / transitionTime);
                sections[neededState]->draw();
            } else {
                sections[state]->reset();
                state = neededState;
                timer = 0;
                sections[state]->setAlpha(255);
                sections[state]->draw();
            }
        }
    }

    return TA_SCREENSTATE_CURRENT;
}

void TA_MainMenuScreen::updateTitle() {
    const float titleY = 10, shift = 8;

    if(state == neededState) {
        titleSprite.setPosition(0, titleY);
        titleSprite.setFrame(state);
        titleSprite.setAlpha(255);
    } else if(timer < transitionTime) {
        titleSprite.setPosition(-shift * timer / transitionTime, titleY);
        titleSprite.setFrame(state);
        titleSprite.setAlpha(255 - 255 * timer / transitionTime);
    } else {
        float delta = 1 - std::min(float(1), (timer - transitionTime) / transitionTime);
        titleSprite.setPosition(-shift * delta, titleY);
        titleSprite.setFrame(neededState);
        titleSprite.setAlpha(255 - 255 * delta);
    }

    titleSprite.draw();
}
