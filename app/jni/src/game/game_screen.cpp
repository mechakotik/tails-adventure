#include "game_screen.h"

void TA_GameScreen::init()
{
    links.character = &character;
    links.tilemap = &tilemap;
    links.camera = &camera;
    links.objectSet = &objectSet;

    character.load(links);
    tilemap.load("maps/pf/pf1.tmx");
    tilemap.setCamera(&camera);
    objectSet.setCollisionTilemap(&tilemap);
    hud.load(links);
    objectSet.setCamera(&camera);
}

TA_ScreenState TA_GameScreen::update()
{
    character.handleInput();
    objectSet.update();
    character.update();
    camera.update(character.isOnGround());

    tilemap.draw(0);
    tilemap.draw(1);
    character.draw();
    objectSet.draw();
    tilemap.draw(2);
    character.drawControls();
    hud.draw();

    if(character.gameOver()) {
        return TA_SCREENSTATE_TITLE;
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_GameScreen::quit()
{

}
