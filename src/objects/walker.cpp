#include "walker.h"
#include "tools.h"
#include "dead_kukku.h"

void TA_Walker::load(TA_Point newPosition, int range, bool flip)
{
    TA_Sprite::load("objects/pf_enemies.png", 24, 32);
    TA_Sprite::loadAnimationsFromFile("objects/pf_enemies_animations.xml");
    hitbox.setRectangle(TA_Point(5, 0), TA_Point(18, 26));

    position = newPosition;
    direction = flip;
    if(range <= 0) {
        alwaysIdle = true;
    }
    else {
        if(!flip) {
            rangeLeft = position.x;
            rangeRight = position.x + range;
        }
        else {
            rangeLeft = position.x - range;
            rangeRight = position.x;
        }
    }
    updatePosition();
}

bool TA_Walker::update()
{
    switch(state) {
        case TA_WALKER_STATE_IDLE:
            updateIdle();
            break;
        case TA_WALKER_STATE_MOVE:
            updateMove();
            break;
        case TA_WALKER_STATE_FIRE:
            updateFire();
            break;
        case TA_WALKER_STATE_MOVE_AWAY:
            updateMoveAway();
            break;
    }

    setPosition(position);
    setFlip(!direction);
    hitbox.setPosition(position);

    int flags;
    objectSet->checkCollision(hitbox, flags);
    if(flags & (TA_COLLISION_EXPLOSION | TA_COLLISION_BOMB)) {
        objectSet->spawnObject<TA_DeadKukku>(position);
        return false;
    }
    return true;
}

void TA_Walker::updateIdle()
{
    setAnimation("walker_idle");
    TA_Point distance = getDistanceToCharacter();
    if (abs(distance.x) <= 140 && abs(distance.y) <= 90) {
        state = TA_WALKER_STATE_MOVE;
    }
}

void TA_Walker::updateMove()
{
    if (alwaysIdle) {
        setAnimation("walker_idle");
    }
    else {
        setAnimation("walker");
        if (!direction) {
            position.x += speed * TA::elapsedTime;
            if (position.x > rangeRight) {
                direction = true;
            }
        }
        else {
            position.x -= speed * TA::elapsedTime;
            if (position.x < rangeLeft) {
                direction = false;
            }
        }
    }
    TA_Point distance = getDistanceToCharacter();
    if(direction == (distance.x < 0) && abs(distance.x) <= 64 && abs(distance.y) <= 16) {
        state = TA_WALKER_STATE_FIRE;
        timer = 0;
    }
}

void TA_Walker::updateFire()
{
    if(!alwaysIdle && timer < standTime) {
        setAnimation("walker_idle");
    }
    else {
        setAnimation("walker_fire");
    }
    timer += TA::elapsedTime;
    if(timer > fireTime) {
        objectSet->spawnObject<TA_WalkerBullet>(position + TA_Point((direction ? -1 : 16), 16), direction);
        state = TA_WALKER_STATE_MOVE_AWAY;
    }
}

void TA_Walker::updateMoveAway()
{
    if (alwaysIdle) {
        state = TA_WALKER_STATE_MOVE;
        return;
    }
    setAnimation("walker");
    if (!direction) {
        position.x -= speed * TA::elapsedTime;
        if (position.x < rangeLeft) {
            state = TA_WALKER_STATE_MOVE;
        }
    }
    else {
        position.x += speed * TA::elapsedTime;
        if (position.x > rangeRight) {
            state = TA_WALKER_STATE_MOVE;
        }
    }
}

TA_Point TA_Walker::getDistanceToCharacter()
{
    TA_Point centeredPosition = position + TA_Point(12, 16);
    TA_Point characterPosition = objectSet->getCharacterPosition();
    return characterPosition - centeredPosition;
}

void TA_WalkerBullet::load(TA_Point newPosition, bool newDirection)
{
    TA_Sprite::load("objects/walker_bullet.png");
    position = newPosition;
    direction = newDirection;
    hitbox.setRectangle(TA_Point(0, 0), TA_Point(7, 7));
}

bool TA_WalkerBullet::update()
{
    if(direction) {
        position.x -= speed;
    }
    else {
        position.x += speed;
    }
    setPosition(position);
    hitbox.setPosition(position);
    int flags;
    objectSet->checkCollision(hitbox, flags);
    if((flags & TA_COLLISION_SOLID) || (flags & TA_COLLISION_HALF_SOLID)) {
        return false;
    }
    return true;
}
