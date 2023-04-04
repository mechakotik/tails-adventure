#include "bullet.h"

void TA_Bullet::load(std::string filename, TA_Point newPosition, TA_Point newVelocity, int frameWidth, int frameHeight)
{
    if(frameWidth != -1) {
        TA_Sprite::load(filename, frameWidth, frameHeight);
    }
    else {
        TA_Sprite::load(filename);
    }
    position = newPosition;
    velocity = newVelocity;
    hitbox.setRectangle(TA_Point(0, 0), TA_Point(getWidth(), getHeight()));
    updatePosition();
}

bool TA_Bullet::update()
{
    position = position + velocity;
    updatePosition();

    int flags = objectSet->checkCollision(hitbox);
    if((flags & TA_COLLISION_SOLID) || (flags & TA_COLLISION_HALF_SOLID)) {
        return false;
    }
    return true;
}

void TA_BirdWalkerBullet::load(TA_Point newPosition, TA_Point newVelocity)
{
    TA_Bullet::load("objects/bird_walker/bullet.png", newPosition, newVelocity, 16, 16);
    TA_Sprite::loadAnimationsFromFile("objects/bird_walker/bullet.xml");
    TA_Sprite::setAnimation("bullet");
}

bool TA_BirdWalkerBullet::update()
{
    return TA_Bullet::update();
}
