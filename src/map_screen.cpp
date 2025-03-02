#include "map_screen.h"
#include <cmath>
#include "save.h"

void TA_MapScreen::init() {
    map.load();
    selector.load();
    TA::sound::playMusic("sound/map.vgm");
    TA::previousLevelPath = "";
}

TA_ScreenState TA_MapScreen::update() {
    TA_ScreenState state = selector.update();

    map.draw();
    map.drawSelectionName(selector.getSelectionName());
    selector.draw();

    return state;
}
