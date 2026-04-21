#include "bomb_thrower.h"
#include "dead_kukku.h"
#include "defs.h"
#include "enemy_bomb.h"
#include "tools.h"

void TA_BombThrower::load(TA_Point position, float leftX, float rightX) {
    this->position = position;
    this->leftX = leftX;
    this->rightX = rightX;

    loadFromToml("objects/bomb_thrower.toml");
    hitbox.setRectangle(TA_Point(1, 2), TA_Point(14, 26));
    collisionType = TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;
    updatePosition();
}

bool TA_BombThrower::update() {
    switch(state) {
        case STATE_IDLE:
            updateIdle();
            break;
        case STATE_WALK_FORWARD:
            updateWalkForward();
            break;
        case STATE_WALK_BACK:
            updateWalkBack();
            break;
        case STATE_ATTACK:
            updateAttack();
            break;
    }

    updatePosition();

    if(shouldBeDestroyed()) {
        destroy();
        return false;
    }
    return true;
}

void TA_BombThrower::updateIdle() {
    timer += TA::elapsedTime;
    TA_Point distance = getDistanceToCharacter();

    if(timer > idleTime && distance.x < 0 && abs(distance.x) <= 160 && abs(distance.y) <= 90) {
        state = STATE_WALK_FORWARD;
        return;
    }

    setAnimation("idle");
    setFlip(false);
}

void TA_BombThrower::updateWalkForward() {
    position.x -= speed * TA::elapsedTime;
    TA_Point distance = getDistanceToCharacter();

    if(position.x < leftX || ((distance.x < 0 && abs(distance.x) <= 54) && abs(distance.y) <= 48)) {
        position.x = std::max(position.x, leftX);
        initAttack();
        return;
    }

    setAnimation("walk");
    setFlip(false);
}

void TA_BombThrower::initAttack() {
    objectSet->spawnObject<TA_EnemyBomb>(
        "objects/enemy_bomb.toml", position + TA_Point(6, 7), TA_Point(1, 6), TA_Point(7, 12), TA_Point(-1.2, -2.4));
    state = STATE_ATTACK;
    setAnimation("throw");
    setFlip(false);
}

void TA_BombThrower::updateAttack() {
    if(!isAnimated()) {
        state = STATE_WALK_BACK;
        timer = 0;
        return;
    }
}

void TA_BombThrower::updateWalkBack() {
    position.x += speed * TA::elapsedTime;
    timer += TA::elapsedTime;

    if(position.x > rightX || timer > walkBackTime) {
        position.x = std::min(position.x, rightX);
        timer = 0;
        state = STATE_IDLE;
        return;
    }

    setAnimation("walk");
    setFlip(true);
}

TA_Point TA_BombThrower::getDistanceToCharacter() {
    TA_Point characterPosition = objectSet->getCharacterPosition();
    TA_Point centeredPosition = position + TA_Point(14, 8);
    return characterPosition - centeredPosition;
}

bool TA_BombThrower::shouldBeDestroyed() {
    return (objectSet->checkCollision(hitbox) & TA_COLLISION_ATTACK) != 0;
}

void TA_BombThrower::destroy() {
    objectSet->spawnObject<TA_DeadKukku>(position - TA_Point(4, 2));
    objectSet->resetInstaShield();
}
