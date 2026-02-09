#include "area_selector.h"
#include <cmath>
#include "controller.h"
#include "resource_manager.h"
#include "save.h"

void TA_AreaSelector::load() {
    controller.load();
    appendPoints();
    fixAreaMask();
    addSelectedArea();
    setActivePoints();
    loadTailsIcon();
    switchSound.load("sound/switch.ogg", TA_SOUND_CHANNEL_SFX1);
}

void TA_AreaSelector::appendPoints() {
    float xOffset = (TA::screenWidth - 256) / 2;
    float yOffset = (TA::screenHeight - 144) / 2;

    const toml::array& tomlMainPoints = TA::resmgr::loadToml("worldmap/points.toml").at("main").as_array();
    const toml::array& tomlSeafoxPoints = TA::resmgr::loadToml("worldmap/points.toml").at("seafox").as_array();
    const toml::array& tomlPoints = TA::save::getSaveParameter("seafox") ? tomlSeafoxPoints : tomlMainPoints;

    for(const toml::value& element : tomlPoints) {
        std::string name = element.at("name").as_string();
        std::string path = (element.contains("path") ? element.at("path").as_string() : "");
        float x = static_cast<float>(element.at("x").as_integer());
        float y = static_cast<float>(element.at("y").as_integer());
        bool battleFortress = element.contains("battle_fortress") && element.at("battle_fortress").as_boolean();
        points.emplace_back(name, path, TA_Point(x + xOffset, y + yOffset), battleFortress);

        if(element.contains("up")) {
            points.back().setNeighbour(TA_DIRECTION_UP, static_cast<int>(element.at("up").as_integer()));
        }
        if(element.contains("down")) {
            points.back().setNeighbour(TA_DIRECTION_DOWN, static_cast<int>(element.at("down").as_integer()));
        }
        if(element.contains("left")) {
            points.back().setNeighbour(TA_DIRECTION_LEFT, static_cast<int>(element.at("left").as_integer()));
        }
        if(element.contains("right")) {
            points.back().setNeighbour(TA_DIRECTION_RIGHT, static_cast<int>(element.at("right").as_integer()));
        }
    }

    pos = TA::save::getSaveParameter("map_selection");
}

void TA_AreaSelector::fixAreaMask() {
    // seafox levels started at 9 instead of 40 before, migrate mask to not break legacy saves
    int64_t areaMask = TA::save::getSaveParameter("area_mask");
    if((areaMask & (1LL << 40)) == 0) {
        for(int add = 0; add <= 20; add++) {
            if((areaMask & (1LL << (9 + add))) != 0) {
                areaMask ^= (1LL << (9 + add));
                areaMask ^= (1LL << (40 + add));
            }
        }
        TA::save::setSaveParameter("area_mask", areaMask);
    }
}

void TA_AreaSelector::addSelectedArea() {
    int64_t areaMask = TA::save::getSaveParameter("area_mask");
    int index = TA::save::getSaveParameter("map_selection");
    if(TA::save::getSaveParameter("seafox")) {
        index += 40;
    }

    if((areaMask & (1LL << index)) == 0) {
        areaMask |= (1LL << index);
        TA::save::setSaveParameter("area_mask", areaMask);
        TA::save::setSaveParameter("last_unlocked", index);
    }
}

void TA_AreaSelector::setActivePoints() {
    int64_t areaMask = TA::save::getSaveParameter("area_mask");
    int add = (TA::save::getSaveParameter("seafox") ? 40 : 0);
    for(int level = 0; level < static_cast<int>(points.size()); level++) {
        if((areaMask & (1LL << (level + add))) != 0) {
            points[level].activate();
        }
    }
}

void TA_AreaSelector::loadTailsIcon() {
    tailsIcon.loadFromToml("worldmap/tails_icon.toml");
    tailsIcon.setAnimation(TA::save::getSaveParameter("seafox") ? "seafox" : "blink");
}

TA_ScreenState TA_AreaSelector::update() {
    controller.update();
    if(controller.isJustChangedDirection()) {
        TA_Direction direction = controller.getDirection();
        if(direction != TA_DIRECTION_MAX && points[pos].getNeighbour(direction) != -1 &&
            points[points[pos].getNeighbour(direction)].isActive()) {
            pos = points[pos].getNeighbour(direction);
            switchSound.play();
        }
    }

    for(int i = 0; i < static_cast<int>(points.size()); i++) {
        if(i != pos && points[i].isActive() && points[i].updateButton()) {
            pos = i;
            switchSound.play();
        }
    }

    TA::save::setSaveParameter("map_selection", pos);

    bfTimer = std::fmod(bfTimer + TA::elapsedTime, TA::pi * 20);
    TA_Point tailsIconPos = points[pos].getPosition() + TA_Point(-2, 8);
    if(points[pos].isBattleFortress()) {
        tailsIconPos += TA_Point(std::cos(bfTimer * 0.1), std::abs(std::sin(bfTimer * 0.1)));
    }
    tailsIcon.setPosition(tailsIconPos);

    if(controller.isJustPressed(TA_BUTTON_A) || controller.isJustPressed(TA_BUTTON_B) || points[pos].updateButton()) {
        TA::levelPath = points[pos].getPath();
        if(TA::levelPath == "") {
            return TA_SCREENSTATE_HOUSE;
        }
        return TA_SCREENSTATE_GAME;
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_AreaSelector::draw() {
    if(TA::save::getSaveParameter("seafox") == 0) {
        for(int pos = 1; pos < static_cast<int>(points.size()); pos++) {
            points[pos].draw();
        }
    }
    tailsIcon.draw();
    controller.draw();
}

std::string TA_AreaSelector::getSelectionName() {
    return points[pos].getName();
}

TA_MapPoint::TA_MapPoint(std::string name, std::string path, TA_Point position, bool battleFortress) {
    this->position = position;
    this->name = name;
    this->path = path;
    this->battleFortress = battleFortress;
    for(int direction = 0; direction < TA_DIRECTION_MAX; direction++) {
        neighbours[direction] = -1;
    }

    sprite.load("worldmap/selection.png");
    sprite.setPosition(position);
    button.setRectangle(position - TA_Point(4, 4), position + TA_Point(16, 16));
}

bool TA_MapPoint::updateButton() {
    button.update();
    return button.isJustPressed();
}

void TA_MapPoint::draw() {
    if(!active) {
        return;
    }

    if(battleFortress) {
        bfTimer = std::fmod(bfTimer + TA::elapsedTime, TA::pi * 20);
        sprite.setPosition(position + TA_Point(std::cos(bfTimer * 0.1), std::abs(std::sin(bfTimer * 0.1))));
    }

    lightTimer = std::fmod(lightTimer + TA::elapsedTime, lightTime * 2);
    if(lightTimer < lightTime) {
        sprite.setAlpha(255 * lightTimer / appearTime);
    } else {
        sprite.setAlpha(255 - 255 * (lightTimer - lightTime) / appearTime);
    }
    sprite.draw();
}
