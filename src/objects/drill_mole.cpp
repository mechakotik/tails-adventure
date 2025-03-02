#include "drill_mole.h"
#include <cmath>
#include "explosion.h"
#include "ring.h"

void TA_DrillMole::load(TA_Point position) {
    this->position = position;
    startY = position.y;

    loadFromToml("objects/drill_mole.toml");
    setAnimation("drill_mole");

    hitbox.setRectangle(TA_Point(5, 0), TA_Point(14, 26));
    TA_Object::updatePosition();
}

bool TA_DrillMole::update() {
    timer += TA::elapsedTime;
    timer = std::fmod(timer, 2 * (idleTime + moveTime));
    updatePosition();

    if(shouldBeDestroyed()) {
        destroy();
        return false;
    }
    return true;
}

void TA_DrillMole::updatePosition() {
    State state;
    double stateTime;
    std::tie(state, stateTime) = getStateAndTime();

    switch(state) {
        case STATE_IDLE_UP:
            position.y = startY;
            break;
        case STATE_IDLE_DOWN:
            position.y = startY + moveRange;
            break;
        case STATE_MOVE_UP:
            position.y = startY + (1 - stateTime) * moveRange;
            break;
        case STATE_MOVE_DOWN:
            position.y = startY + stateTime * moveRange;
            break;
    }

    TA_Object::updatePosition();
}

std::pair<TA_DrillMole::State, double> TA_DrillMole::getStateAndTime() {
    if(timer < idleTime) {
        return {STATE_IDLE_UP, timer / idleTime};
    }
    if(timer < idleTime + moveTime) {
        return {STATE_MOVE_DOWN, (timer - idleTime) / moveTime};
    }
    if(timer < 2 * idleTime + moveTime) {
        return {STATE_IDLE_DOWN, (timer - (idleTime + moveTime)) / idleTime};
    }
    return {STATE_MOVE_UP, (timer - (2 * idleTime + moveTime)) / moveTime};
}

bool TA_DrillMole::shouldBeDestroyed() {
    if(getStateAndTime().first != STATE_IDLE_UP) {
        return false;
    }
    if(objectSet->checkCollision(hitbox) & TA_COLLISION_ATTACK) {
        return true;
    }
    return false;
}

void TA_DrillMole::destroy() {
    objectSet->spawnObject<TA_Explosion>(position + TA_Point(3, 5));
    objectSet->resetInstaShield();
    if(objectSet->enemyShouldDropRing()) {
        objectSet->spawnObject<TA_Ring>(position + TA_Point(7, 9));
    }
}
