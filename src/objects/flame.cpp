#include "flame.h"
#include <cmath>
#include "tools.h"

void TA_Flame::load(TA_Point position, double startSpeed) {
    this->position = position;
    speed = -startSpeed;
    startY = position.y;

    TA_Sprite::load("objects/flame.png");
    hitbox.setRectangle(TA_Point(0, 0), TA_Point(7, 8));
    updatePosition();
}

bool TA_Flame::update() {
    speed += gravity * TA::elapsedTime;
    position.y += std::max(-maxSpeed, std::min(maxSpeed, speed)) * TA::elapsedTime;
    updatePosition();

    if(position.y > startY) {
        return false;
    }
    return true;
}

void TA_FlameLauncher::load(TA_Point position, double startSpeed) {
    this->position = position;
    this->startSpeed = startSpeed;
}

bool TA_FlameLauncher::update() {
    if(active) {
        timer += TA::elapsedTime;
        if(timer > launchPeriod) {
            objectSet->spawnObject<TA_Flame>(position, startSpeed);
            timer = std::fmod(timer, launchPeriod);
        }
    } else {
        TA_Point distance = objectSet->getCharacterPosition() - position;
        if(abs(distance.x) <= 96 && abs(distance.y) <= 96) {
            active = true;
        }
    }

    return true;
}
