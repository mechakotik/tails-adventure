#include "main_menu_screen.h"
#include "data_select_section.h"
#include "sound.h"

void TA_MainMenuScreen::init()
{
    TA::sound::playMusic("sound/password.vgm");
    controller.load();
    sections[TA_MAIN_MENU_DATA_SELECT] = std::make_unique<TA_DataSelectSection>(TA_DataSelectSection(&controller));
    sections[TA_MAIN_MENU_DATA_SELECT]->load();
    state = neededState = TA_MAIN_MENU_DATA_SELECT;
}

TA_ScreenState TA_MainMenuScreen::update()
{
    neededState = sections[state]->update();
    sections[state]->draw();

    return TA_SCREENSTATE_CURRENT;
}
