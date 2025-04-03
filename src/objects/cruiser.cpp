#include "cruiser.h"
#include "sea_fox.h"

void TA_Cruiser::load() {
    loadFromToml("objects/cruiser/cruiser.toml");
    hitbox.setRectangle({16, 45}, {164, 94});
    position = {288, 62};

    hitboxVector.assign(2, HitboxVectorElement());
    hitboxVector[0].hitbox.setRectangle({-16, 0}, {0, TA::screenHeight});
    hitboxVector[0].collisionType = TA_COLLISION_SOLID;
    hitboxVector[1].hitbox.setRectangle({0, 0}, {16, TA::screenHeight});
    hitboxVector[1].collisionType = TA_COLLISION_SOLID;

    updatePosition();
    updateBorderPosition();
    lockPosition = {32, 240 - TA::screenHeight};
    objectSet->getLinks().camera->setLockPosition(lockPosition);
}

bool TA_Cruiser::update() {
    switch(state) {
        case State::IDLE:
            updateIdle();
            break;
        case State::ACTIVE:
            updateActive();
            break;
        case State::DESTROYED:
            updateDestroyed();
            break;
    }

    updatePosition();
    updateBorderPosition();
    return true;
}

void TA_Cruiser::updateBorderPosition() {
    hitboxVector[0].hitbox.setPosition(lockPosition);
    hitboxVector[1].hitbox.setPosition(lockPosition + TA_Point(TA::screenWidth, 0));
}

void TA_Cruiser::updateIdle() {
    if(objectSet->getLinks().camera->isLocked()) {
        TA::sound::playMusic("sound/boss.vgm");
        state = State::ACTIVE;
    }
}

void TA_Cruiser::updateActive() {
    position.x += TA::elapsedTime;
    float center = position.x + 104;
    if(cameraNormalized) {
        lockPosition.x += TA::elapsedTime;
    } else {
        lockPosition.x += TA::elapsedTime * 1.5;
        if(lockPosition.x > center - TA::screenWidth / 2) {
            cameraNormalized = true;
        }
    }

    objectSet->getLinks().seaFox->setVelocityAdd({1, 0});
    objectSet->getLinks().camera->setLockPosition(lockPosition);
    objectSet->getLinks().camera->forceLockX();
}

void TA_Cruiser::updateDestroyed() {

}
