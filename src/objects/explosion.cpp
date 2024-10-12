#include "explosion.h"
#include "tools.h"
#include "error.h"

void TA_Explosion::load(TA_Point position, int newDelay, TA_ExplosionType type)
{
    TA_Sprite::load("objects/explosion.png", 16, 16);
    TA_Sprite::loadAnimationsFromFile("objects/explosion.xml");
    TA_Sprite::setPosition(position);
    TA_Sprite::setAnimation("explosion");

    this->type = type;
    delay = newDelay;
    hitbox.setRectangle(TA_Point(-2, -2), TA_Point(17, 17));
    hitbox.setPosition(position);
}

bool TA_Explosion::update()
{
    timer += TA::elapsedTime;
    if(!TA_Sprite::isAnimated()) {
        return false;
    }
    return true;
}

int TA_Explosion::getCollisionType()
{
    int collisionType = TA_COLLISION_TRANSPARENT;
    if(type == TA_EXPLOSION_CHARACTER && delay == 0) {
        collisionType |= TA_COLLISION_ATTACK;
    }
    else if(type == TA_EXPLOSION_ENEMY) {
        collisionType = TA_COLLISION_DAMAGE;
    }
    return collisionType;
}

void TA_Explosion::draw()
{
    if(timer >= delay) {
        TA_Sprite::draw();
    }
}
