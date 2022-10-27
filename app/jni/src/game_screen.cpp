#include "game_screen.h"
#include "globals.h"

void TA_GameScreen::init()
{
    links.character = &character;
    links.tilemap = &tilemap;
    links.camera = &camera;

    character.load(links);
    tilemap.load("maps/pf/pf1.tmx");
    tilemap.setCamera(&camera);
}

TA_ScreenState TA_GameScreen::update()
{
    character.update();
    camera.update();

    tilemap.draw(0);
    tilemap.draw(1);
    character.draw();
    tilemap.draw(2);
    character.drawControls();

    return TA_SCREENSTATE_CURRENT;
}

void TA_GameScreen::quit()
{

}
