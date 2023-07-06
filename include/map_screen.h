#ifndef TA_MAP_SCREEN_H
#define TA_MAP_SCREEN_H

#include "screen.h"
#include "ingame_map.h"
#include "area_selector.h"

class TA_MapScreen : public TA_Screen {
private:
    TA_InGameMap map;
    TA_AreaSelector selector;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override {}
};

#endif // TA_MAP_SCREEN_H
