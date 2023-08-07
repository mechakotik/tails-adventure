#ifndef TA_GAME_SCREEN_H
#define TA_GAME_SCREEN_H

#include "screen.h"
#include "tilemap.h"
#include "camera.h"
#include "geometry.h"
#include "character.h"
#include "links.h"
#include "hud.h"
#include "object_set.h"

class TA_GameScreen : public TA_Screen {
private:
    TA_Tilemap tilemap, backgroundTilemap;
    TA_Camera camera;
    TA_Character character;
    TA_ObjectSet objectSet;
    TA_Links links;
    TA_Hud hud;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override;
};


#endif // TA_GAME_SCREEN_H
