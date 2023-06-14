#include "flame.h"
#include "tools.h"

void TA_Flame::load(TA_Point position)
{
    this->position = position;
    startY = position.y;
    TA_Sprite::load("objects/flame.png");
    hitbox.setRectangle(TA_Point(0, 0), TA_Point(7, 8));
    updatePosition();
}

bool TA_Flame::update()
{
    velocity.y += gravity * TA::elapsedTime;
    position = position + velocity;
    updatePosition();

    if(position.y > startY) {
        return false;
    }
    return true;
}

void TA_FlameLauncher::load(TA_Point position)
{
    this->position = position;
}

bool TA_FlameLauncher::update()
{
    if(active) {
        timer += TA::elapsedTime;
        if(timer > launchPeriod) {
            objectSet->spawnObject<TA_Flame>(position);
            timer = std::fmod(timer, launchPeriod);
        }
    }
    else {
        TA_Point distance = objectSet->getCharacterPosition() - position;
        if(abs(distance.x) <= 96 && abs(distance.y) <= 96) {
            active = true;
        }
    }

    return true;
}
