#include "nezu.h"
#include "explosion.h"
#include "tools.h"

void TA_Nezu::load(TA_Point position)
{
    this->position = position;

    TA_Sprite::load("objects/nezu.png", 16, 16);
    TA_Sprite::loadAnimationsFromFile("objects/nezu.xml");
    TA_Sprite::setAnimation("idle");

    hitbox.setRectangle(TA_Point(2, 3), TA_Point(13, 14));
    updatePosition();
}

bool TA_Nezu::update()
{
    switch(state) {
        case STATE_IDLE:
            updateIdle();
            break;
        case STATE_WALK:
            updateWalk();
            break;
        case STATE_ATTACK:
            updateAttack();
            break;
    }

    updatePosition();
    if(objectSet->checkCollision(hitbox) & (TA_COLLISION_EXPLOSION | TA_COLLISION_BOMB)) {
        objectSet->spawnObject<TA_Explosion>(position, 0, TA_EXPLOSION_NEUTRAL);
        return false;
    }
    return true;
}

void TA_Nezu::updateIdle()
{
    setAnimation("idle");
    setFlip(false);
    TA_Point distance = getDistanceToCharacter();
    if(abs(distance.x) <= 54 && abs(distance.y) <= 32) {
        state = STATE_WALK;
        direction = (distance.x > 0);
    }
}

void TA_Nezu::updateWalk()
{
    setAnimation("walk");
    setFlip(direction);

    TA_Point newPosition = position;
    newPosition.x += speed * (direction ? 1 : -1) * TA::elapsedTime;
    if(isBadPosition(newPosition) || isCloseToCharacter()) {
        state = STATE_ATTACK;
        bombPlaced = false;
        timer = 0;
    }
    else if(abs(getDistanceToCharacter().x) >= 96) {
        state = STATE_IDLE;
    }
    else {
        position = newPosition;
    }
}

void TA_Nezu::updateAttack()
{
    setAnimation("idle");
    setFlip(false);

    timer += TA::elapsedTime;
    if(timer > placeTime && !bombPlaced) {
        bombPlaced = true;
    }
    if(timer > attackTime) {
        direction = !direction;
        state = STATE_WALK;
    }
}

TA_Point TA_Nezu::getDistanceToCharacter()
{
    TA_Point centeredPosition = position + TA_Point(8, 8);
    TA_Point characterPosition = objectSet->getCharacterPosition();
    return characterPosition - centeredPosition;
}

bool TA_Nezu::isBadPosition(TA_Point position)
{
    TA_Polygon groundHitbox, wallHitbox;
    if(direction) {
        groundHitbox.setRectangle(TA_Point(17, 18), TA_Point(18, 19));
        wallHitbox.setRectangle(TA_Point(16, 0), TA_Point(18, 10));
    }
    else {
        groundHitbox.setRectangle(TA_Point(-2, 18), TA_Point(-1, 19));
        wallHitbox.setRectangle(TA_Point(-2, 0), TA_Point(0, 10));
    }
    groundHitbox.setPosition(position);
    wallHitbox.setPosition(position);

    const int solidMask = TA_COLLISION_SOLID | TA_COLLISION_HALF_SOLID;
    int flags = objectSet->checkCollision(groundHitbox);
    if((flags & solidMask) == 0) {
        return true;
    }
    flags = objectSet->checkCollision(wallHitbox);
    if(flags & solidMask) {
        return true;
    }
    return false;
}

bool TA_Nezu::isCloseToCharacter()
{
    TA_Point distance = getDistanceToCharacter();
    if(direction != (distance.x > 0)) {
        return false;
    }
    return abs(distance.x) <= 14 && abs(distance.y) <= 16;
}
