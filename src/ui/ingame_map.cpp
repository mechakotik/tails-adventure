#include "ingame_map.h"
#include "resource_manager.h"
#include "save.h"
#include "tools.h"

void TA_InGameMap::load() {
    float xOffset = (TA::screenWidth - 256) / 2;
    float yOffset = (TA::screenHeight - 144) / 2;

    mapSprite.loadFromToml("worldmap/map.toml");
    mapSprite.setPosition({xOffset, yOffset});
    mapSprite.setAnimation("map");

    battleFortressSprite.loadFromToml("worldmap/battle_fortress.toml");
    toml::value basePosValue = TA::resmgr::loadToml("worldmap/points.toml").at("battle_fortress");
    battleFortressBasePos = TA_Point(basePosValue.at("x").as_integer(), basePosValue.at("y").as_integer());
    battleFortressBasePos += TA_Point(xOffset, yOffset);

    namePlateSprite.load("worldmap/name_plate.png");
    namePlateSprite.setPosition(16, TA::screenHeight - 24);

    font.loadFont("fonts/area.toml");

    for(int i = 0; i < 2; i++) {
        dolphinSprites[i].loadFromToml("worldmap/dolphin.toml");
        dolphinSprites[i].setAnimation("dolphin");
    }

    dolphinSprites[0].setPosition(xOffset + 76, yOffset + 24);
    dolphinSprites[1].setPosition(xOffset + 172, yOffset + 48);

    for(int i = 0; i < 3; i++) {
        birdSprites[i].loadFromToml("worldmap/bird.toml");
        birdSprites[i].setAnimation("bird");
    }

    birdSprites[0].setPosition(xOffset + 68, yOffset + 88);
    birdSprites[1].setPosition(xOffset + 64, yOffset + 96);
    birdSprites[2].setPosition(xOffset + 72, yOffset + 96);

    const toml::array& tomlMainPoints = TA::resmgr::loadToml("worldmap/points.toml").at("main").as_array();
    const toml::array& tomlSeafoxPoints = TA::resmgr::loadToml("worldmap/points.toml").at("seafox").as_array();

    int64_t areaMask = TA::save::getSaveParameter("area_mask");
    int idx = 0;
    for(const toml::value& element : tomlMainPoints) {
        bool battleFortress = element.contains("battle_fortress") && element.at("battle_fortress").as_boolean();
        if(battleFortress && (areaMask & (1LL << idx)) != 0) {
            battleFortressNeeded = true;
        }
        idx++;
    }

    idx = 40;
    for(const toml::value& element : tomlSeafoxPoints) {
        bool battleFortress = element.contains("battle_fortress") && element.at("battle_fortress").as_boolean();
        if(battleFortress && (areaMask & (1LL << idx)) != 0) {
            battleFortressNeeded = true;
        }
        idx++;
    }
}

void TA_InGameMap::draw() {
    drawBackground();
    mapSprite.draw();
    for(int i = 0; i < 2; i++) {
        dolphinSprites[i].draw();
    }
    for(int i = 0; i < 3; i++) {
        birdSprites[i].draw();
    }

    timer = std::fmod(timer + TA::elapsedTime, TA::pi * 20);
    if(battleFortressNeeded) {
        battleFortressSprite.setPosition(
            battleFortressBasePos + TA_Point(std::cos(timer * 0.1), std::abs(std::sin(timer * 0.1))));
        battleFortressSprite.draw();
    }
}

void TA_InGameMap::drawBackground() {
    float yOffset = (TA::screenHeight - 144) / 2;
    TA::drawScreenRect(17, 136, 221, 255);
    TA::drawRect({0, 0}, {static_cast<float>(TA::screenWidth), yOffset + 9}, 187, 238, 238, 255);
}

void TA_InGameMap::drawSelectionName(std::string name) {
    namePlateSprite.draw();
    TA_Point textPosition = namePlateSprite.getPosition() + TA_Point(3, 4);
    TA_Point offset = {0, 0};
    if(font.getTextWidth(name) > namePlateSprite.getWidth() - 8) {
        offset = {-1, 0};
    }
    font.drawText(textPosition, name, offset);
}
