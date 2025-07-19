#include "devmenu_screen.h"
#include "save.h"

void TA_DevmenuScreen::init() {
    controller.load();
    levels.emplace_back("maps/pf/pf1");
    levels.emplace_back("maps/pf/pf2");
    levels.emplace_back("maps/pf/pf3");
    levels.emplace_back("maps/vt/vt1");
    levels.emplace_back("maps/vt/vt2");
    levels.emplace_back("maps/vt/vt3");
    levels.emplace_back("maps/pm/pm1");
    levels.emplace_back("maps/pm/pm2");
    levels.emplace_back("maps/pm/pm3");
    levels.emplace_back("maps/pm/pm4");
    levels.emplace_back("maps/ci/ci1");
    levels.emplace_back("maps/ci/ci2");
    levels.emplace_back("maps/ci/ci3");
    levels.emplace_back("maps/cf/cf1");
    levels.emplace_back("maps/cf/cf2");
    levels.emplace_back("maps/lr/lr1");
    levels.emplace_back("maps/lr/lr2");
    levels.emplace_back("maps/lr/lr3");
    levels.emplace_back("maps/lr/lr4");
    levels.emplace_back("maps/lr/lr5");
    levels.emplace_back("maps/lr/lr6");
    levels.emplace_back("maps/lr/lr7");
    levels.emplace_back("maps/lr/lr8");
    levels.emplace_back("maps/lr/lr9");
    levels.emplace_back("maps/lc/lc1");
    levels.emplace_back("maps/lc/lc2");
    levels.emplace_back("maps/gi/gi1");
    levels.emplace_back("maps/gi/gi2");
    levels.emplace_back("maps/pv/pv1");
    levels.emplace_back("maps/pv/pv2");
    levels.emplace_back("maps/cc/cc1");
    levels.emplace_back("maps/cc/cc2");
    normalFont.loadFont("fonts/devmenu.toml");
    selectedFont.loadFont("fonts/devmenu_selected.toml");
}

TA_ScreenState TA_DevmenuScreen::update() {
    controller.update();

    auto addMod = [&](int& x, int add, int min, int max) {
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
                } else if(controller.getDirection() == TA_DIRECTION_RIGHT) {
                    addMod(levelPosition, 1, 0, int(levels.size()) - 1);
                }
            }
            break;
    }

    if(menuPosition == TA_DEVMENU_ELEMENT_LEVEL) {
        selectedFont.drawTextCentered(22, "Level: " + levels[levelPosition], {-1, 0});
    } else {
        normalFont.drawTextCentered(22, "Level: " + levels[levelPosition], {-1, 0});
    }

    if(controller.isJustPressed(TA_BUTTON_A) || controller.isJustPressed(TA_BUTTON_B)) {
        TA::levelPath = levels[levelPosition];
        TA::save::repairSave("save_0");
        TA::save::setCurrentSave("save_0");
        return TA_SCREENSTATE_GAME;
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_DevmenuScreen::quit() {}
