#include "explosion.h"
#include "engine/tools.h"
#include "engine/error.h"

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
        hitbox.setRectangle(TA_Point(0, 0), TA_Point(15, 15));
    }
    return true;
}

void TA_Explosion::draw()
{
    timer += TA::elapsedTime;
    if(timer >= delay) {
        TA_Sprite::draw();
    }
}
