#include "pushable_object.h"
#include "engine/tools.h"

void TA_PushableObject::load(std::string filename, TA_Point newPosition)
{
    TA_Sprite::load(filename);
    position = newPosition;
    hitbox.setRectangle(TA_Point(0, 0), TA_Point(getWidth(), getHeight()));
}

bool TA_PushableObject::update()
{
    TA_Polygon leftHitbox, rightHitbox;
    leftHitbox.setRectangle(TA_Point(-0.1, getHeight() / 2), TA_Point(0, getHeight()));
    rightHitbox.setRectangle(TA_Point(getWidth(), getHeight() / 2), TA_Point(getWidth() + 0.1, getHeight()));
    leftHitbox.setPosition(position);
    rightHitbox.setPosition(position);

    double speed = TA::elapsedTime * 0.33;
    if(objectSet->checkCollision(leftHitbox) & TA_COLLISION_CHARACTER) {
        velocity.x = speed;
    }
    else if(objectSet->checkCollision(rightHitbox) & TA_COLLISION_CHARACTER) {
        velocity.x = -speed;
    }
    else {
        velocity.x = 0;
    }
    velocity.y += grv;

    int flags = moveAndCollide(TA_Point(0, 0), TA_Point(getWidth(), getHeight()), velocity);
    if(flags & TA_GROUND_COLLISION) {
        velocity.y = 0;
    }

    updatePosition();
    return true;
}

bool TA_PushableObject::checkPawnCollision(TA_Polygon &hitbox)
{
    int flags = objectSet->checkCollision(hitbox);
    if((flags & TA_COLLISION_SOLID) || (flags & TA_COLLISION_HALF_SOLID)) {
        return true;
    }
    return false;
}

void TA_PushableSpring::load(TA_Point newPosition)
{
    TA_PushableObject::load("objects/spring.png", newPosition);
    HitboxVectorElement element;
    element.hitbox.setRectangle(TA_Point(0.1, 0), TA_Point(getWidth() - 0.1, 0));
    element.collisionType = TA_COLLISION_SPRING;
    hitboxVector.push_back(element);
}
