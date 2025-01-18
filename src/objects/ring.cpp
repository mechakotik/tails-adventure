#include "ring.h"
#include "tools.h"
#include "error.h"
#include "character.h"

void TA_Ring::load(TA_Point position, TA_Point velocity, double delay) {
    this->position = position;
    this->velocity = velocity;
    this->delay = delay;

    TA_Sprite::load("objects/ring.png", 8, 8);
    TA_Sprite::loadAnimationsFromFile("objects/ring_animations.xml");
    TA_Sprite::setAnimation("ring");
    hitbox.setRectangle({0, 0}, {7, 7});
    ringSound.load("sound/ring.ogg", TA_SOUND_CHANNEL_SFX2);
    setPosition(position);
}

void TA_Ring::load(TA_Point position, double speed) {
    load(position, {0, speed}, 0);
}

bool TA_Ring::checkPawnCollision(TA_Polygon &hitbox) {
    int flags;
    objectSet->checkCollision(hitbox, flags);
    return flags & (TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP);
}

bool TA_Ring::update() {
    if(collected) {
        return isAnimated();
    }

    velocity.y += grv * TA::elapsedTime;
    TA_Point topLeft{0, 0}, bottomRight{8, 8};
    int flags = moveAndCollide(topLeft, bottomRight, velocity * TA::elapsedTime);
    setPosition(position);

    if(velocity.x > 0) {
        velocity.x = std::max(0.0, velocity.x - drag * TA::elapsedTime);
    }
    else {
        velocity.x = std::min(0.0, velocity.x + drag * TA::elapsedTime);
    }

    if((flags & TA_GROUND_COLLISION) && velocity.y > 0) {
        velocity.y *= -slowdown;
        if(velocity.y > -0.5) {
            velocity.y = 0;
        }
    }
    if(flags & TA_WALL_COLLISION) {
        velocity.x *= -1;
    }
    if((flags & TA_CEIL_COLLISION) && velocity.y < 0) {
        velocity.y *= -1;
    }

    if(timer > delay) {
        hitbox.setPosition(position);
        objectSet->checkCollision(hitbox, flags);
        if(flags & TA_COLLISION_CHARACTER) {
            ringSound.play();
            objectSet->addRings(1);
            collected = true;
            timer = 0;
            setAnimation("flick");
            return true;
        }
    }

    timer += TA::elapsedTime;
    if(timer > maxTime) {
        return false;
    }
    return true;
}
