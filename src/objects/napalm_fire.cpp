#include "napalm_fire.h"
#include <iostream>

void TA_NapalmFire::load(TA_Point position, double xsp)
{
    this->position = position;
    velocity = {xsp, 0};

    TA_Sprite::load("objects/napalm_fire.png", 8, 31);
    TA_Sprite::loadAnimationsFromFile("objects/napalm_fire.xml");
    TA_Sprite::setAnimation("fire");

    TA_Polygon explosionHitbox;
    explosionHitbox.setRectangle(TA_Point(0, 0), TA_Point(8, 28));
    hitboxVector.push_back({explosionHitbox, TA_COLLISION_EXPLOSION});

    TA_Polygon napalmHitbox;
    napalmHitbox.setRectangle(TA_Point(-4, -16), TA_Point(4, 0));
    hitboxVector.push_back({napalmHitbox, TA_COLLISION_NAPALM});

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
    int flags = moveAndCollide(topLeft, bottomRight, velocity * TA::elapsedTime, true);
    if(flags & TA_WALL_COLLISION) {
        velocity.x = 0;
    }

    updatePosition();
    
    if(!TA_Sprite::isAnimated()) {
        return false;
    }
    return true;
}
