#include "main_menu_screen.h"
#include "data_select_section.h"
#include "options_section.h"
#include "sound.h"

void TA_MainMenuScreen::init()
{
    TA::sound::playMusic("sound/password.vgm");
    controller.load();

    sections[TA_MAIN_MENU_DATA_SELECT] = std::make_unique<TA_DataSelectSection>(TA_DataSelectSection(&controller));
    sections[TA_MAIN_MENU_DATA_SELECT]->load();

    sections[TA_MAIN_MENU_OPTIONS] = std::make_unique<TA_OptionsSection>(TA_OptionsSection(&controller));
    sections[TA_MAIN_MENU_OPTIONS]->load();

    state = neededState = TA_MAIN_MENU_DATA_SELECT;
}

TA_ScreenState TA_MainMenuScreen::update()
{
    controller.update();

    if(state == neededState) {
        neededState = sections[state]->update();
        sections[state]->draw();
    }
    else {
        if(neededState == TA_MAIN_MENU_EXIT) {
            sections[state]->draw();
            return TA_SCREENSTATE_MAP;
        }
        else {
            timer += TA::elapsedTime;
            if(timer < transitionTime) {
                sections[state]->setAlpha(255 - 255 * timer / transitionTime);
                sections[state]->draw();
            }
            else if(timer < transitionTime * 2) {
                sections[neededState]->setAlpha(255 * (timer - transitionTime) / transitionTime);
                sections[neededState]->draw();
            }
            else {
                state = neededState;
                timer = 0;
                sections[state]->setAlpha(255);
                sections[state]->draw();
            }
        }
    }

    return TA_SCREENSTATE_CURRENT;
}
