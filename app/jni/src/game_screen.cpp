#include "game_screen.h"
#include "globals.h"

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
    int direction = characterController.update();
    switch(direction) {
        case TA_DIRECTION_UP:
            followPosition.y -= gElapsedTime;
            break;
        case TA_DIRECTION_DOWN:
            followPosition.y += gElapsedTime;
            break;
        case TA_DIRECTION_LEFT:
            followPosition.x -= gElapsedTime;
            break;
        case TA_DIRECTION_RIGHT:
            followPosition.x += gElapsedTime;
            break;
        default:
            break;
    }

    camera.update();

    tilemap.draw(0);
    characterController.draw();

    return TA_SCREENSTATE_CURRENT;
}

void TA_GameScreen::quit()
{

}
