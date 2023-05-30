#include "explosion.h"
#include "tools.h"
#include "error.h"

void TA_Explosion::load(TA_Point position, int newDelay)
{
    TA_Sprite::load("effects/explosion.png", 16, 16);
    TA_Sprite::loadAnimationsFromFile("effects/animations.xml");
    TA_Sprite::setPosition(position);
    TA_Sprite::setAnimation("explosion");
    delay = newDelay;
    hitbox.setRectangle(TA_Point(4, 4), TA_Point(11, 11));
    hitbox.setPosition(position);
}

bool TA_Explosion::update()
{
    if(!TA_Sprite::isAnimated()) {
        return false;
    }
    if(timer >= delay + 8) {
        hitbox.setRectangle(TA_Point(-2, -2), TA_Point(17, 17));
    }
    return true;
}

TA_CollisionType TA_Explosion::getCollisionType()
{
    int type = TA_COLLISION_EXPLOSION;
    if(delay == 0) {
        type |= TA_COLLISION_EXPLOSION_FIRST;
    }
    return TA_CollisionType(type);
}

void TA_Explosion::draw()
{
    timer += TA::elapsedTime;
    if(timer >= delay) {
        TA_Sprite::draw();
    }
}
