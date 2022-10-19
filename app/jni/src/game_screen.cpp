#include "game_screen.h"

void TA_GameScreen::init()
{
    tilemap.load("maps/pf/pf1.tmx");
    followPosition = TA_Point(0, 128);
    camera.setFollowPosition(&followPosition);
    tilemap.setCamera(&camera);
    characterController.load();
}

TA_ScreenState TA_GameScreen::update()
{
    if(characterController.update() == 1) {
        followPosition.y --;
    }

    camera.update();

    tilemap.draw(0);
    characterController.draw();

    return TA_SCREENSTATE_CURRENT;
}

void TA_GameScreen::quit()
{

}
