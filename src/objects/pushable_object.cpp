#include "pushable_object.h"
#include "character.h"
#include "tilemap.h"
#include "tools.h"

void TA_PushableObject::load(std::string filename, TA_Point newPosition) {
    TA_Sprite::load(filename);
    position = newPosition;
    hitbox.setRectangle(TA_Point(0.33, 0), TA_Point(getWidth() - 0.33, getHeight()));
    collisionType = TA_COLLISION_PUSHABLE;
}

bool TA_PushableObject::update() {
    TA_Rect leftHitbox, rightHitbox;
    leftHitbox.setRectangle(TA_Point(-0.1, getHeight() / 2), TA_Point(0, getHeight()));
    rightHitbox.setRectangle(TA_Point(getWidth(), getHeight() / 2), TA_Point(getWidth() + 0.1, getHeight()));
    leftHitbox.setPosition(position);
    rightHitbox.setPosition(position);

    float speed = 0.33;
    velocity.x = 0;
    if(objectSet->getLinks().character->isOnGround()) {
        if(objectSet->checkCollision(leftHitbox) & TA_COLLISION_CHARACTER) {
            velocity.x = speed;
        } else if(objectSet->checkCollision(rightHitbox) & TA_COLLISION_CHARACTER) {
            velocity.x = -speed;
        }
    }
    velocity.y += grv * TA::elapsedTime;

    // TODO: actually fix pushable objects collision
    auto [delta, flags] = objectSet->moveAndCollide(position, TA_Point(0, 0), TA_Point(getWidth(), getHeight()),
        velocity * TA::elapsedTime, TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP, (TA::levelPath == "maps/pf/pf1"));
    position += delta;
    if(flags & TA_GROUND_COLLISION) {
        velocity.y = 0;
    }

    updatePosition();
    return true;
}

void TA_PushableSpring::load(TA_Point newPosition) {
    TA_PushableObject::load("objects/spring.png", newPosition);
    springBounceSprite.load("objects/spring_bounce.png");
    springBounceSprite.setCamera(objectSet->getLinks().camera);

    HitboxVectorElement element;
    element.hitbox.setRectangle({0.5, -0.1}, {getWidth() - 0.5, 0});
    element.collisionType = TA_COLLISION_SPRING;
    hitboxVector.push_back(element);
}

bool TA_PushableSpring::update() {
    springBounceSprite.setPosition(position - TA_Point(0, 12));
    return TA_PushableObject::update();
}

void TA_PushableSpring::draw() {
    if(objectSet->getLinks().character->isJumpingOnSpring() &&
        objectSet->getLinks().character->getJumpTime() < bounceTime) {
        springBounceSprite.draw();
    } else {
        TA_PushableObject::draw();
    }
}
