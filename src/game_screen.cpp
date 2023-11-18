#include "game_screen.h"
#include "save.h"

void TA_GameScreen::init()
{
    links.character = &character;
    links.tilemap = &tilemap;
    links.camera = &camera;
    links.objectSet = &objectSet;
    links.hud = &hud;
    links.controller = &controller;

    controller.load();
    character.load(links);
    objectSet.setLinks(links);
    tilemap.load(TA::levelPath + ".tmx");
    tilemap.setCamera(&camera);
    hud.load(links);
    objectSet.load(TA::levelPath + ".xml");
    character.setSpawnPoint(objectSet.getCharacterSpawnPoint(), objectSet.getCharacterSpawnFlip());
    TA::previousLevelPath = TA::levelPath;
}

TA_ScreenState TA_GameScreen::update()
{
    controller.update();
    hud.update();
    objectSet.updateMusic();

    if(!hud.isPaused()) {
        character.handleInput();
        objectSet.update();
        character.update();
        camera.update(character.isOnGround(), character.isJumpingOnSpring() || character.isOnStrongWind() || character.isUsingSpeedBoots());
    }

    character.setPaused(hud.isPaused());
    tilemap.setUpdateAnimation(!hud.isPaused());
    objectSet.setPaused(hud.isPaused());

    tilemap.draw(0);
    objectSet.draw(0);
    character.draw();
    objectSet.draw(1);
    tilemap.draw(1);
    objectSet.draw(2);
    hud.draw();

    if(hud.getTransition() != TA_SCREENSTATE_CURRENT) {
        return hud.getTransition();
    }
    if(character.gameOver()) {
        return TA_SCREENSTATE_GAMEOVER;
    }
    if(character.isTeleported()) {
        TA::save::saveToLatest();
        return TA_SCREENSTATE_HOUSE;
    }
    if(objectSet.getTransition() != TA_SCREENSTATE_CURRENT) {
        if(objectSet.getTransition() == TA_SCREENSTATE_MAP) {
            TA::save::saveToLatest();
        }
        return objectSet.getTransition();
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_GameScreen::quit()
{

}
