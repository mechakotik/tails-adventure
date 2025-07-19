#include "map_screen.h"
#include "save.h"

void TA_MapScreen::init() {
    if(TA::save::getSaveParameter("rings") <= 0) {
        TA::save::setSaveParameter("map_position", 0);
        TA::save::setSaveParameter("seafox", false);
        setMaxRings();
    }

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

void TA_MapScreen::setMaxRings() {
    long long itemMask = TA::save::getSaveParameter("item_mask");
    int rings = 8;
    for(int num = 29; num <= 34; num++) {
        if(itemMask & (1ll << num)) {
            rings += 2;
        }
    }
    TA::save::setSaveParameter("rings", rings);
}
