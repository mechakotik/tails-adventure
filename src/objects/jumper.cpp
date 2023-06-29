#include "jumper.h"
#include "dead_kukku.h"

void TA_Jumper::load(TA_Point position)
{
    this->position = position;

    TA_Sprite::load("objects/jumper.png", 16, 31);
    TA_Sprite::loadAnimationsFromFile("objects/jumper.xml");

    hitbox.setRectangle(TA_Point(1, 1), TA_Point(15, 31));
    updatePosition();
}

bool TA_Jumper::update()
{
    updateDirection();

    switch(state) {
        case STATE_IDLE:
            updateIdle();
            break;
        case STATE_AIM:
            updateAim();
            break;
        case STATE_JUMP:
            updateJump();
            break;
    }

    updatePosition();

    if(shouldBeDestroyed()) {
        destroy();
        return false;
    }
    return true;
}

void TA_Jumper::updateIdle()
{
    setAnimation("jump");
}

void TA_Jumper::updateAim()
{
    setAnimation("aim");
}

void TA_Jumper::updateJump()
{
    setAnimation("jump");
}

void TA_Jumper::updateDirection()
{
    TA_Point distance = getDistanceToCharacter();
    direction = (distance.x > 0);
    setFlip(direction);
}

bool TA_Jumper::shouldBeDestroyed()
{
    if(objectSet->checkCollision(hitbox) & (TA_COLLISION_BOMB | TA_COLLISION_EXPLOSION)) {
        return true;
    }
    return false;
}

void TA_Jumper::destroy()
{
    objectSet->spawnObject<TA_DeadKukku>(position - TA_Point(4, 1));
}
