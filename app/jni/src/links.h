#ifndef TA_LINKS_H
#define TA_LINKS_H

class TA_Character;
class TA_Tilemap;
class TA_Camera;
class TA_ObjectSet;

struct TA_GameScreenLinks {
    TA_Character *character;
    TA_Tilemap *tilemap;
    TA_Camera *camera;
    TA_ObjectSet *objectSet;
};

#endif // TA_LINKS_H
