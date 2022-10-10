#include "game_screen.h"

void TA_GameScreen::init()
{
    tilemap.load("maps/pf/pf1.tmx");
}

TA_ScreenState TA_GameScreen::update()
{
    tilemap.draw(0);
    return TA_SCREENSTATE_CURRENT;
}

void TA_GameScreen::quit()
{

}
