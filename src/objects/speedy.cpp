#include "speedy.h"
#include "tools.h"
#include "character.h"

void TA_Speedy::load()
{
    loadSprite();
    initShow();
}

void TA_Speedy::loadSprite()
{
    TA_Sprite::load("objects/speedy.png", 24, 32);
    TA_Sprite::loadAnimationsFromFile("objects/speedy.xml");
    hitbox.setRectangle(TA_Point(6, 10), TA_Point(20, 30));
}

void TA_Speedy::initShow()
{
    state = STATE_SHOW;
    position = {200, 3883};
    velocity = startShowVelocity;
    setAnimation("show");
}

bool TA_Speedy::update()
{
    switch(state) {
        case STATE_SHOW:
            updateShow();
            break;
        case STATE_IDLE:
            updateIdle();
            break;
        case STATE_ATTACK:
            updateAttack();
            break;
        case STATE_FLY_UP_SIMPLE:
            updateFlyUpSimple();
            break;
        case STATE_FLY_UP:
            updateFlyUp();
            break;
    }

    updatePosition();
    updateFlip();
    return true;
}

void TA_Speedy::updateShow()
{
    if(waiting) {
        timer += TA::elapsedTime;
        if(timer > showWaitTime) {
            waiting = false;
        }
    }
    else {
        velocity.y -= gravity * TA::elapsedTime;
        position = position + velocity * TA::elapsedTime;
    }
    
    TA_Point cameraPosition = objectSet->getLinks().camera->getPosition();

    if((position.x < cameraPosition.x - 24 || position.y < cameraPosition.y - 32) && velocity.x < 0) {
        velocity = startShowVelocity;
        velocity.x *= -1;
        waiting = true;
        timer = 0;
    }
    
    else if(position.x > cameraPosition.x + 180 && position.y < cameraPosition.y - 32) {
        initIdle();
    }
}

void TA_Speedy::initIdle()
{
    state = STATE_IDLE;
    position = {-32, 0};
}

void TA_Speedy::updateIdle()
{
    if(objectSet->getCharacterPosition().y < 3776) {
        objectSet->playBossMusic();
        initAttack();
    }
}

void TA_Speedy::initAttack()
{
    state = STATE_ATTACK;
    position.x = objectSet->getCharacterPosition().x - 12;
    position.y = objectSet->getLinks().camera->getPosition().y - 32;
    setAnimation("attack");
}

void TA_Speedy::updateAttack()
{
    position.y += attackSpeed * TA::elapsedTime;
    if(position.y > objectSet->getLinks().camera->getPosition().y + 200) {
        initFlyUp();
    }
}

void TA_Speedy::initFlyUp()
{
    if(objectSet->getCharacterPosition().x > 128) {
        position.x = 64;
    }
    else {
        position.x = 168;
    }
    position.y = objectSet->getLinks().camera->getPosition().y + 176;
    velocity = TA_Point(0, -flyUpSpeed);
    setAnimation("fly_up");

    if(TA::random::next() % 2 == 0) {
        flyUpPhase = 0;
        state = STATE_FLY_UP;
    }
    else {
        state = STATE_FLY_UP_SIMPLE;
    }
}

void TA_Speedy::updateFlyUpSimple()
{
    position = position + velocity * TA::elapsedTime;
    if(position.y < objectSet->getLinks().camera->getPosition().y - 64) {
        initAttack();
    }
}

void TA_Speedy::updateFlyUp()
{
    if(flyUpPhase == 0) {
        double cameraY = objectSet->getLinks().camera->getPosition().y;
        if(position.y < cameraY + 24) {
            flyUpPhase = 1;
            flyUpPhase2Y = cameraY - 8;
        }
    }
    else if(flyUpPhase == 1 && position.y < flyUpPhase2Y) {
        flyUpPhase = 2;
    }

    if(flyUpPhase != 0) {
        if(flyUpPhase == 1) {
            velocity = {0, -flyUpPhase1Speed};
        }
        else {
            velocity = {0, -flyUpPhase2Speed};
        }
        setAnimation("show");
    }

    updateFlyUpSimple();
}

void TA_Speedy::updateFlip()
{
    if(state == STATE_FLY_UP && getCurrentFrame() == 7) {
        setFlip(position.x < 128);
    }
    else {
        setFlip(getAnimationFrame() >= 2);
    }
}

TA_CollisionType TA_Speedy::getCollisionType()
{
    if(state == STATE_ATTACK) {
        return TA_COLLISION_DAMAGE;
    }
    return TA_COLLISION_TRANSPARENT;
}
