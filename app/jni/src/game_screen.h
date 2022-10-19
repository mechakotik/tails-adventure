#ifndef TA_GAME_SCREEN_H
#define TA_GAME_SCREEN_H

#include "screen.h"
#include "tilemap.h"
#include "camera.h"
#include "geometry.h"
#include "character_controller.h"

class TA_GameScreen : public TA_Screen {
private:
    TA_Tilemap tilemap;
    TA_Camera camera;
    TA_Point followPosition;
    TA_CharacterController characterController;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override;
};


#endif // TA_GAME_SCREEN_H
