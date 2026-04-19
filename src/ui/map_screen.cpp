#include "map_screen.h"
#include "save.h"
#include "tools.h"

void TA_MapScreen::init() {
    if(TA::save::getSaveParameter("rings") <= 0) {
        TA::save::setSaveParameter("map_position", 0);
        TA::save::setSaveParameter("seafox", false);
        setMaxRings();
    }

    selector.load();
    map.load();
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
    int64_t itemMask = TA::save::getSaveParameter("item_mask");
    int rings = 8;
    for(int num = 29; num <= 34; num++) {
        if((itemMask & (1LL << num)) != 0) {
            rings += 2;
        }
    }
    TA::save::setSaveParameter("rings", rings);
}
