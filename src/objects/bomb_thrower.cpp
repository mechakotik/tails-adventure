#include "bomb_thrower.h"
#include "dead_kukku.h"
#include "explosion.h"

void TA_BombThrower::load(TA_Point position, double leftX, double rightX)
{
    this->position = position;
    this->leftX = leftX;
    this->rightX = rightX;

    TA_Sprite::load("objects/bomb_thrower.png", 16, 27);
    TA_Sprite::loadAnimationsFromFile("objects/bomb_thrower.xml");

    hitbox.setRectangle(TA_Point(1, 2), TA_Point(14, 26));
    updatePosition();
}

bool TA_BombThrower::update()
{
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

void TA_BombThrower::updateIdle()
{
    timer += TA::elapsedTime;
    TA_Point distance = getDistanceToCharacter();
    
    if(timer > idleTime && distance.x < 0 && abs(distance.x) <= 160 && abs(distance.y) <= 90) {
        state = STATE_WALK_FORWARD;
        return;
    }

    setAnimation("idle");
    setFlip(false);
}

void TA_BombThrower::updateWalkForward()
{
    position.x -= speed * TA::elapsedTime;
    TA_Point distance = getDistanceToCharacter();

    if(position.x < leftX || (distance.x < 0 && abs(distance.x) <= 54) && abs(distance.y) <= 48) {
        position.x = std::max(position.x, leftX);
        initAttack();
        return;
    }

    setAnimation("walk");
    setFlip(false);
}

void TA_BombThrower::initAttack()
{
    objectSet->spawnObject<TA_EnemyBomb>(position + TA_Point(6, 7));
    state = STATE_ATTACK;
    setAnimation("throw");
    setFlip(false);
}

void TA_BombThrower::updateAttack()
{
    if(!isAnimated()) {
        state = STATE_WALK_BACK;
        timer = 0;
        return;
    }
}

void TA_BombThrower::updateWalkBack()
{
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

TA_Point TA_BombThrower::getDistanceToCharacter()
{
    TA_Point characterPosition = objectSet->getCharacterPosition();
    TA_Point centeredPosition = position + TA_Point(14, 8);
    return characterPosition - centeredPosition;
}

bool TA_BombThrower::shouldBeDestroyed()
{
    if(objectSet->checkCollision(hitbox) & (TA_COLLISION_BOMB | TA_COLLISION_EXPLOSION | TA_COLLISION_INSTA_SHIELD)) {
        return true;
    }
    return false;
}

void TA_BombThrower::destroy()
{
    objectSet->spawnObject<TA_DeadKukku>(position - TA_Point(4, 2));
    objectSet->resetInstaShield();
}

void TA_EnemyBomb::load(TA_Point position)
{
    this->position = position;
    TA_Sprite::load("objects/enemy_bomb.png");
    hitbox.setRectangle(topLeft - TA_Point(0.5, 0.5), bottomRight + TA_Point(0.5, 0.5));
    updatePosition();
}

bool TA_EnemyBomb::update()
{
    velocity.y += grv * TA::elapsedTime;
    int flags = moveAndCollide(topLeft, bottomRight, velocity * TA::elapsedTime);

    if(flags != 0) {
        objectSet->spawnObject<TA_Explosion>(position - TA_Point(4, 1), 0, TA_EXPLOSION_ENEMY);
        return false;
    }

    updatePosition();
    return true;
}

bool TA_EnemyBomb::checkPawnCollision(TA_Polygon &hitbox)
{
    if(objectSet->checkCollision(hitbox) & (TA_COLLISION_SOLID | TA_COLLISION_HALF_SOLID | TA_COLLISION_CHARACTER)) {
        return true;
    }
    return false;
}
