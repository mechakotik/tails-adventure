#include "explosion.h"
#include "engine/globals.h"

void TA_Explosion::load(TA_Point position, int newDelay)
{
    TA_Sprite::load("effects/explosion.png", 16, 16);
    TA_Sprite::loadAnimationsFromFile("effects/animations.xml");
    TA_Sprite::setPosition(position);
    TA_Sprite::setAnimation("explosion");
    delay = newDelay;
}

bool TA_Explosion::update()
{
    if(!TA_Sprite::isAnimated()) {
        return false;
    }
    return true;
}

void TA_Explosion::draw()
{
    timer += gElapsedTime;
    if(timer >= delay) {
        TA_Sprite::draw();
    }
}
