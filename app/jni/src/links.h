#ifndef TA_LINKS_H
#define TA_LINKS_H

class TA_Character;
class TA_Tilemap;
class TA_Camera;

struct TA_GameScreenLinks {
    TA_Character *character;
    TA_Tilemap *tilemap;
    TA_Camera *camera;
};

#endif // TA_LINKS_H
