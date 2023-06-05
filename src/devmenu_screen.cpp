#include "devmenu_screen.h"

void TA_DevmenuScreen::init()
{
    controller.load();
    levels.emplace_back("maps/pf/pf1");
    levels.emplace_back("maps/pf/pf2");
    levels.emplace_back("maps/pf/pf3");
    levels.emplace_back("maps/vt/vt1");
    normalFont.load("fonts/devmenu.png", 7, 9);
    normalFont.setMapping(mapping);
    selectedFont.load("fonts/devmenu_selected.png", 7, 9);
    selectedFont.setMapping(mapping);
}

TA_ScreenState TA_DevmenuScreen::update()
{
    controller.update();

    auto addMod = [&](int &x, int add, int min, int max) {
        x += add;
        if(x < min) {
            x = max;
        }
        if(x > max) {
            x = min;
        }
    };

    if(controller.isJustChangedDirection()) {
        switch(controller.getDirection()) {
            case TA_DIRECTION_UP:
                addMod(menuPosition, -1, 0, TA_DEVMENU_ELEMENT_MAX - 1);
                break;

            case TA_DIRECTION_DOWN:
                addMod(menuPosition, 1, 0, TA_DEVMENU_ELEMENT_MAX - 1);
                break;

            default:
                break;
        }
    }

    normalFont.drawTextCentered(10, "Tails Adventure dev menu", {-1, 0});

    switch(menuPosition) {
        case TA_DEVMENU_ELEMENT_LEVEL:
            if(controller.isJustChangedDirection()) {
                if(controller.getDirection() == TA_DIRECTION_LEFT) {
                    addMod(levelPosition, -1, 0, int(levels.size()) - 1);
                }
                else if(controller.getDirection() == TA_DIRECTION_RIGHT) {
                    addMod(levelPosition, 1, 0, int(levels.size()) - 1);
                }
            }
            break;
    }

    if(menuPosition == TA_DEVMENU_ELEMENT_LEVEL) {
        selectedFont.drawTextCentered(22, "Level: " + levels[levelPosition], {-1, 0});
    }
    else {
        normalFont.drawTextCentered(22, "Level: " + levels[levelPosition], {-1, 0});
    }

    controller.draw();

    if(controller.isJustPressed(TA_BUTTON_A) || controller.isJustPressed(TA_BUTTON_B)) {
        TA::levelPath = levels[levelPosition];
        return TA_SCREENSTATE_GAME;
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_DevmenuScreen::quit()
{

}
