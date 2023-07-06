#include <cmath>
#include "map_screen.h"
#include "save.h"

void TA_MapScreen::init()
{
    map.load();
    selector.load();
}

TA_ScreenState TA_MapScreen::update()
{
    TA_ScreenState state = selector.update();
    
    map.draw();
    selector.draw();

    return state;
}
