#include "game_screen.h"

void TA_GameScreen::init()
{
    links.character = &character;
    links.tilemap = &tilemap;
    links.camera = &camera;
    links.objectSet = &objectSet;

    character.load(links);
    tilemap.load(TA::levelPath + ".tmx");
    tilemap.setCamera(&camera);
    objectSet.setCollisionTilemap(&tilemap);
    hud.load(links);
    objectSet.setCamera(&camera);
    objectSet.setCharacterHitbox(character.getHitbox());
    objectSet.load(TA::levelPath + ".xml");
    character.setSpawnPoint(objectSet.getCharacterSpawnPoint(), objectSet.getCharacterSpawnFlip());
    TA::previousLevelPath = TA::levelPath;
}

TA_ScreenState TA_GameScreen::update()
{
    character.handleInput();
    objectSet.update();
    character.update();
    camera.update(character.isOnGround(), character.isJumpingOnSpring());

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
    if(objectSet.getTransition() != TA_SCREENSTATE_CURRENT) {
        return objectSet.getTransition();
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_GameScreen::quit()
{

}
