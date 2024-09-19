#ifndef TA_GAME_SCREEN_H
#define TA_GAME_SCREEN_H

#include "screen.h"
#include "tilemap.h"
#include "camera.h"
#include "geometry.h"
#include "character.h"
#include "sea_fox.h"
#include "controller.h"
#include "links.h"
#include "hud.h"
#include "object_set.h"

class TA_GameScreen : public TA_Screen {
private:
    TA_Tilemap tilemap;
    TA_Camera camera;
    TA_Character character;
    TA_SeaFox seaFox;
    TA_Controller controller;
    TA_ObjectSet objectSet;
    TA_Links links;
    TA_Hud hud;

    bool isSeaFox = false;
    double timer = 0;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override;
};


#endif // TA_GAME_SCREEN_H
