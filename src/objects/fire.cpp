#include "fire.h"
#include "tools.h"

void TA_Fire::load(TA_Point position, bool flip)
{
    this->position = position;
    this->flip = flip;

    TA_Sprite::load("objects/fire.png", 24, 8);
    TA_Sprite::loadAnimationsFromFile("objects/fire.xml");
    TA_Sprite::setFlip(flip);

    updateHitbox();
    updatePosition();
}

bool TA_Fire::update()
{
    if(!isAnimated()) {
        timer += TA::elapsedTime;
        if(timer > waitTime) {
            setAnimation("fire");
            timer = 0;
        }
    }

    updateHitbox();
    updatePosition();
    return true;
}

void TA_Fire::updateHitbox()
{
    double left = 0;
    switch(getCurrentFrame()) {
        case 0:
            left = 0;
            break;
        case 1:
            left = 8;
            break;
        case 2:
            left = 11;
            break;
        case 3:
            left = 19;
            break;
        default:
            break;
    }
    if(flip) {
        hitbox.setRectangle(TA_Point(0, 0), TA_Point(24 - left, 8));
    }
    else {
        hitbox.setRectangle(TA_Point(left, 0), TA_Point(24, 8));
    }
}
