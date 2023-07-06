#ifndef TA_INGAME_MAP_H
#define TA_INGAME_MAP_H

#include "tilemap.h"

class TA_InGameMap {
private:
    TA_Tilemap tilemap;

public:
    void load();
    void draw();
};

#endif // TA_INGAME_MAP_H
