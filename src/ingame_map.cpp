#include "ingame_map.h"

void TA_InGameMap::load()
{
    tilemap.load("worldmap/map.tmx");
}

void TA_InGameMap::draw()
{
    tilemap.draw(0);
}
