#include "hover_pod.h"

void TA_HoverPod::load(TA_Point newPosition, int range, bool flip)
{
    TA_Sprite::load("objects/pf_enemies.png", 24, 32);
    TA_Sprite::loadAnimationsFromFile("objects/pf_enemies_animations.xml");
    TA_Sprite::setAnimation("hover_pod");
    hitbox.setRectangle(TA_Point(4, 1), TA_Point(20, 30));

    position = newPosition;
    direction = flip;
    if(!flip) {
        rangeLeft = position.x;
        rangeRight = position.x + range;
    }
    else {
        rangeLeft = position.x - range;
        rangeRight = position.x;
    }
}

bool TA_HoverPod::update()
{
    TA_Point characterPosition = objectSet->getCharacterPosition();

    if(idle) {
        if (abs(position.x - characterPosition.x) <= 140 &&
            abs(position.y - characterPosition.y) <= 80) {
            idle = false;
        }
        return true;
    }

    if (!direction) {
        position.x += speed;
        if (position.x > rangeRight) {
            direction = true;
        }
    }
    else {
        position.x -= speed;
        if (position.x < rangeLeft) {
            direction = false;
        }
    }

    setPosition(position);
    setFlip(!direction);
    hitbox.setPosition(position);

    int flags;
    objectSet->checkCollision(hitbox, flags);
    if(flags & TA_COLLISION_EXPLOSION) {
        objectSet->spawnDeadKukku(position);
        return false;
    }
    return true;
}
