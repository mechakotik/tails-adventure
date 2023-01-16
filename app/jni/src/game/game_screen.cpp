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

    objectSet.spawnBreakableBlock(TA_Point(1120, 256));
    objectSet.spawnBreakableBlock(TA_Point(1120, 272));
    objectSet.spawnBreakableBlock(TA_Point(1120, 288));
    objectSet.spawnBreakableBlock(TA_Point(1232, 336));
    objectSet.spawnBreakableBlock(TA_Point(1248, 336));
    objectSet.spawnBreakableBlock(TA_Point(1296, 336));
    objectSet.spawnBreakableBlock(TA_Point(1312, 336));
    objectSet.spawnBreakableBlock(TA_Point(1264, 384), true);
    objectSet.spawnBreakableBlock(TA_Point(1280, 384), true);
}

TA_ScreenState TA_GameScreen::update()
{
    character.handleInput();
    objectSet.update();
    character.update();
    camera.update(character.isOnGround());

    tilemap.draw(0);
    tilemap.draw(1);
    objectSet.draw(0);
    character.draw();
    objectSet.draw(1);
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
