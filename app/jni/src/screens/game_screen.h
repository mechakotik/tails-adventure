#ifndef TA_GAME_SCREEN_H
#define TA_GAME_SCREEN_H

#include "engine/screen.h"
#include "engine/tilemap.h"
#include "engine/camera.h"
#include "engine/geometry.h"
#include "screens/game/character.h"
#include "engine/links.h"
#include "screens/game/hud.h"
#include "engine/object_set.h"

class TA_GameScreen : public TA_Screen {
private:
    TA_Tilemap tilemap;
    TA_Camera camera;
    TA_Character character;
    TA_ObjectSet objectSet;
    TA_GameScreenLinks links;
    TA_Hud hud;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override;
};


#endif // TA_GAME_SCREEN_H
