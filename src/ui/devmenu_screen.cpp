#include "devmenu_screen.h"
#include "save.h"
#include "tools.h"

void TA_DevmenuScreen::init() {
    controller.load();
    normalFont.loadFont("fonts/devmenu.toml");
    selectedFont.loadFont("fonts/devmenu_selected.toml");

    levels = {
        "maps/pf/pf1",
        "maps/pf/pf2",
        "maps/pf/pf3",
        "maps/vt/vt1",
        "maps/vt/vt2",
        "maps/vt/vt3",
        "maps/pm/pm1",
        "maps/pm/pm2",
        "maps/pm/pm3",
        "maps/pm/pm4",
        "maps/ci/ci1",
        "maps/ci/ci2",
        "maps/ci/ci3",
        "maps/cf/cf1",
        "maps/cf/cf2",
        "maps/lr/lr1",
        "maps/lr/lr2",
        "maps/lr/lr3",
        "maps/lr/lr4",
        "maps/lr/lr5",
        "maps/lr/lr6",
        "maps/lr/lr7",
        "maps/lr/lr8",
        "maps/lr/lr9",
        "maps/lc/lc1",
        "maps/lc/lc2",
        "maps/gi/gi1",
        "maps/gi/gi2",
        "maps/pv/pv1",
        "maps/pv/pv2",
        "maps/cc/cc1",
        "maps/cc/cc2",
        "maps/cc/cc3",
        "maps/bf/bf1",
        "maps/bf/bf2",
        "maps/bf/bf3",
        "maps/bf/bf4",
        "maps/bf/bf5",
    };
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
