#include <cmath>
#include "map_screen.h"
#include "save.h"

void TA_MapScreen::init()
{
    map.load();
    selector.load();

    beginMusic.load("sound/map_begin.ogg", TA_SOUND_CHANNEL_MUSIC);
    loopMusic.load("sound/map_loop.ogg", TA_SOUND_CHANNEL_MUSIC, true);
    beginMusic.play();

    TA::previousLevelPath = "";
}

TA_ScreenState TA_MapScreen::update()
{
    if(!TA::sound::isPlaying(TA_SOUND_CHANNEL_MUSIC)) {
        loopMusic.play();
    }

    TA_ScreenState state = selector.update();
    
    map.draw();
    map.drawSelectionName(selector.getSelectionName());
    selector.draw();

    return state;
}
