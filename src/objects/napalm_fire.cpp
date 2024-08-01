#include "napalm_fire.h"
#include <iostream>

void TA_NapalmFire::load(TA_Point position, double xsp)
{
    this->position = position;
    velocity = {xsp, 0};

    TA_Sprite::load("objects/napalm_fire.png", 8, 31);
    TA_Sprite::loadAnimationsFromFile("objects/napalm_fire.xml");
    TA_Sprite::setAnimation("fire");

    updatePosition();
}

bool TA_NapalmFire::checkPawnCollision(TA_Polygon &hitbox)
{
    int flags = objectSet->checkCollision(hitbox);
    if(flags & (TA_COLLISION_SOLID | TA_COLLISION_HALF_SOLID | TA_COLLISION_PUSHABLE)) {
        return true;
    }
    return false;
}

bool TA_NapalmFire::update()
{
    int flags = moveAndCollide(topLeft, bottomRight, velocity * TA::elapsedTime);
    if(flags & TA_WALL_COLLISION) {
        velocity.x = 0;
    }

    updatePosition();
    
    if(!TA_Sprite::isAnimated()) {
        return false;
    }
    return true;
}
