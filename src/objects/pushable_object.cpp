#include "pushable_object.h"
#include <algorithm>
#include "character.h"
#include "defs.h"
#include "tools.h"

void TA_PushableObject::load(std::string filename, TA_Point newPosition) {
    TA_Sprite::load(filename);
    position = newPosition;
    hitbox.setRectangle(TA_Point(0.33, 0), TA_Point(getWidth() - 0.33, getHeight()));
    setCollisionEnabled(true);
}

bool TA_PushableObject::update() {
    if(gloveState == GloveState::CARRIED) {
        updatePosition();
        return true;
    }
    if(gloveState == GloveState::THROWN) {
        return updateThrown();
    }

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
    auto [delta, flags] = objectSet->moveAndCollide(position, TA_Point(1, 0), TA_Point(getWidth() - 1, getHeight()),
        velocity * TA::elapsedTime, TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP, (TA::levelPath == "maps/pf/pf1"));
    position += delta;
    if(flags & TA_GROUND_COLLISION) {
        velocity.y = 0;
        setCollisionEnabled(true);
    } else {
        setCollisionEnabled(false);
        collisionType = TA_COLLISION_DYNAMIC_SOLID | TA_COLLISION_ATTACK | TA_COLLISION_NO_BREAK;
    }

    updatePosition();
    return true;
}

bool TA_PushableObject::updateThrown() {
    velocity.y += grv * TA::elapsedTime;

    auto [delta, flags] = objectSet->moveAndCollide(position, TA_Point(1, 0), TA_Point(getWidth() - 1, getHeight()),
        velocity * TA::elapsedTime,
        TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP | TA_COLLISION_PUSHABLE | TA_COLLISION_MOVING_PLATFORM);
    position += delta;

    if(flags & TA_WALL_COLLISION) {
        velocity.x = 0;
    }
    if(flags & TA_CEIL_COLLISION) {
        velocity.y = std::max(0.0F, velocity.y);
    }
    if((flags & TA_GROUND_COLLISION) && velocity.y > 0) {
        velocity.x *= 0.5F;
        velocity.y *= -0.75F;
        if(velocity.y > -0.75F) {
            velocity = {0, 0};
            gloveState = GloveState::NORMAL;
            setCollisionEnabled(true);
        }
    }

    updatePosition();
    return true;
}

void TA_PushableObject::setCollisionEnabled(bool enabled) {
    collisionType = enabled ? TA_COLLISION_PUSHABLE : TA_COLLISION_TRANSPARENT;
}

void TA_PushableObject::startCarry() {
    gloveState = GloveState::CARRIED;
    velocity = {0, 0};
    setCollisionEnabled(false);
}

void TA_PushableObject::setCarriedPosition(TA_Point newPosition) {
    position = newPosition;
    updatePosition();
}

void TA_PushableObject::throwFromCarry(TA_Point throwVelocity) {
    gloveState = GloveState::THROWN;
    velocity = throwVelocity;
    setCollisionEnabled(false);
    collisionType = TA_COLLISION_ATTACK | TA_COLLISION_NO_BREAK;
    updatePosition();
}

void TA_PushableObject::releaseCarry() {
    gloveState = GloveState::NORMAL;
    velocity = {0, 0};
    setCollisionEnabled(true);
    updatePosition();
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

void TA_PushableSpring::setCollisionEnabled(bool enabled) {
    TA_PushableObject::setCollisionEnabled(enabled);
    if(!hitboxVector.empty()) {
        hitboxVector[0].collisionType = enabled ? TA_COLLISION_SPRING : TA_COLLISION_TRANSPARENT;
    }
}

void TA_PushableSpring::draw() {
    if(!isCarried() && !isThrown() && objectSet->getLinks().character->isJumpingOnSpring() &&
        objectSet->getLinks().character->getJumpTime() < bounceTime) {
        springBounceSprite.draw();
    } else {
        TA_PushableObject::draw();
    }
}
