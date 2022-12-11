#include "explosion.h"

void TA_Explosion::load(TA_Point position)
{
    TA_Sprite::load("effects/explosion.png", 16, 16);
    TA_Sprite::loadAnimationsFromFile("effects/animations.xml");
    TA_Sprite::setPosition(position);
    TA_Sprite::setAnimation("explosion");
}

bool TA_Explosion::update()
{
    if(!TA_Sprite::isAnimated()) {
        return false;
    }
    return true;
}
