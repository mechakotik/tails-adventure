#ifndef TA_LINKS_H
#define TA_LINKS_H

class TA_Character;
class TA_Tilemap;
class TA_Camera;
class TA_ObjectSet;
class TA_Controller;
class TA_Hud;

struct TA_Links {
    TA_Character *character = nullptr;
    TA_Tilemap *tilemap = nullptr;
    TA_Camera *camera = nullptr;
    TA_ObjectSet *objectSet = nullptr;
    TA_Controller *controller = nullptr;
    TA_Hud *hud;
};

#endif // TA_LINKS_H
