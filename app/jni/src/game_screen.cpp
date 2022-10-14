#include "game_screen.h"

void TA_GameScreen::init()
{
    tilemap.load("maps/pf/pf1.tmx");
    followPosition = TA_Point(0, 128);
    camera.setFollowPosition(&followPosition);
    tilemap.setCamera(&camera);
}

TA_ScreenState TA_GameScreen::update()
{
    followPosition.x ++;
    followPosition.y ++;

    camera.update();
    tilemap.draw(0);

    return TA_SCREENSTATE_CURRENT;
}

void TA_GameScreen::quit()
{

}
