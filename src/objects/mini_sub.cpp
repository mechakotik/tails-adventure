#include "mini_sub.h"
#include "dead_kukku.h"

void TA_MiniSub::load(TA_Point position)
{
    TA_Sprite::load("objects/mini_sub.png", 24, 32);
    TA_Sprite::loadAnimationsFromFile("objects/mini_sub.xml");
    TA_Sprite::setAnimation("mini_sub");

    hitbox.setRectangle(TA_Point(5, 2), TA_Point(19, 30));
    this->position = position;
    updatePosition();
}

bool TA_MiniSub::update()
{
    if(idle) {
        updateIdle();
    }
    else {
        updateAttack();
    }

    if(objectSet->checkCollision(hitbox) & (TA_COLLISION_CHARACTER | TA_COLLISION_EXPLOSION)) {
        objectSet->spawnObject<TA_DeadKukku>(position);
        return false;
    }

    updatePosition();

    if(timer > minExistTime && !objectSet->isVisible(hitbox)) {
        return false;
    }
    return true;
}

void TA_MiniSub::updateIdle()
{
    if(objectSet->isVisible(hitbox)) {
        idle = false;
        flip = (position.x < objectSet->getCharacterPosition().x);
        setFlip(flip);
    }
}

void TA_MiniSub::updateAttack()
{
    position.x -= speed * TA::elapsedTime * (flip ? -1 : 1);
    timer += TA::elapsedTime;
}
