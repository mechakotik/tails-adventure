#include "game_screen.h"
#include "resource_manager.h"
#include "save.h"

void TA_GameScreen::init() {
    const toml::value& table = TA::resmgr::loadToml(TA::levelPath + ".toml");
    if(table.contains("level") && table.at("level").contains("mode") && table.at("level").at("mode").is_string()) {
        mode = table.at("level").at("mode").as_string();
    } else {
        mode = "ground";
    }

    isSeaFox = (mode != "ground");
    isSeaFoxGround = (mode == "seafox_ground");

    if(isSeaFox) {
        links.seaFox = &seaFox;
    } else {
        links.character = &character;
    }

    links.tilemap = &tilemap;
    links.camera = &camera;
    links.objectSet = &objectSet;
    links.hud = &hud;
    links.controller = &controller;

    controller.load();
    controller.setMode(TA_ONSCREEN_CONTROLLER_GAME);

    if(isSeaFox) {
        seaFox.load(links);
        if(mode == "seafox_ground") {
            seaFox.setGroundMode();
        }
    } else {
        character.load(links);
    }

    objectSet.setLinks(links);
    tilemap.load(TA::levelPath + ".tmx");
    tilemap.setCamera(&camera);
    hud.load(links);
    objectSet.load(TA::levelPath + ".toml");

    if(isSeaFox) {
        seaFox.setSpawnPoint(objectSet.getCharacterSpawnPoint(), objectSet.getCharacterSpawnFlip());
    } else {
        character.setSpawnPoint(objectSet.getCharacterSpawnPoint(), objectSet.getCharacterSpawnFlip());
    }

    TA::previousLevelPath = TA::levelPath;
    timer = TA::save::getSaveParameter("time");
}

TA_ScreenState TA_GameScreen::update() {
    timer += TA::elapsedTime;
    TA::save::setSaveParameter("time", timer);

    controller.update();
    hud.update();

    if(!hud.isPaused()) {
        if(!isSeaFox) {
            character.handleInput();
        }
        objectSet.update();

        if(isSeaFox) {
            seaFox.update();
            camera.update(!isSeaFoxGround, false);
        } else {
            character.update();
            camera.update(character.isOnGround(), character.isFastCamera());
        }
    }

    if(isSeaFox) {
        seaFox.setUpdateAnimation(!hud.isPaused());
    } else {
        character.setPaused(hud.isPaused());
    }

    tilemap.setUpdateAnimation(!hud.isPaused());
    objectSet.setPaused(hud.isPaused());

    tilemap.draw(0);
    objectSet.draw(0);

    if(isSeaFox) {
        seaFox.draw();
    } else if(!objectSet.isNight()) {
        character.draw();
    }

    objectSet.draw(1);
    tilemap.draw(1);

    if(!isSeaFox && objectSet.isNight()) {
        character.draw();
    }

    objectSet.draw(2);
    hud.draw();
    controller.draw();

    if(hud.getTransition() != TA_SCREENSTATE_CURRENT) {
        return hud.getTransition();
    }
    if((!isSeaFox && character.gameOver()) || (isSeaFox && seaFox.gameOver())) {
        return TA_SCREENSTATE_GAMEOVER;
    }
    if(!isSeaFox && character.isTeleported()) {
        return TA_SCREENSTATE_HOUSE;
    }
    if(objectSet.getTransition() != TA_SCREENSTATE_CURRENT) {
        return objectSet.getTransition();
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_GameScreen::quit() {}
