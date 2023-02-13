#include "dead_kukku.h"
#include "engine/tools.h"

void TA_DeadKukku::load(TA_Point newPosition)
{
    TA_Sprite::load("objects/pf_enemies.png", 24, 32);
    TA_Sprite::loadAnimationsFromFile("objects/pf_enemies_animations.xml");
    TA_Sprite::setAnimation("death");
    position = newPosition;
}

bool TA_DeadKukku::update()
{
    velocity.y += grv;
    position = position + velocity;
    setPosition(position);
    timer += TA::elapsedTime;
    if(timer > deathTime) {
        return false;
    }
    return true;
}
