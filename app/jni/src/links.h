#ifndef TA_LINKS_H
#define TA_LINKS_H

class TA_Character;
class TA_Tilemap;
class TA_Camera;
class TA_ObjectSet;
class TA_CommonController;
class TA_Hud;

struct TA_GameScreenLinks {
    TA_Character *character;
    TA_Tilemap *tilemap;
    TA_Camera *camera;
    TA_ObjectSet *objectSet;
    TA_CommonController *controller;
    TA_Hud *hud;
};

#endif // TA_LINKS_H