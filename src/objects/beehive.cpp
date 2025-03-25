#include "beehive.h"
#include "explosion.h"
#include "geometry.h"
#include "sprite.h"
#include "tilemap.h"
#include "tools.h"

void TA_BeeHive::load(TA_Point position) {
    loadFromToml("objects/beehive.toml");
    setAnimation("beehive");

    hitbox.setRectangle(TA_Point(2, 2), TA_Point(14, 14));
    this->position = position;
    updatePosition();
}

bool TA_BeeHive::update() {
    if(!destroyed && beeCount < 2) {
        if(timer > cooldownTime) {
            objectSet->spawnObject<TA_Bee>(position, this);
            beeCount++;
            timer = 0;
        } else {
            timer += TA::elapsedTime;
        }
    }

    if(!destroyed && (objectSet->checkCollision(hitbox) & (TA_COLLISION_ATTACK | TA_COLLISION_NAPALM)) != 0) {
        objectSet->spawnObject<TA_StrongBee>(position);
        objectSet->spawnObject<TA_Explosion>(position, 0, TA_EXPLOSION_NEUTRAL);
        destroyed = true;
        setAlpha(0);
    }

    if(destroyed && beeCount == 0) {
        return false;
    }
    return true;
}

int TA_BeeHive::getCollisionType() {
    if(!destroyed) {
        return TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;
    }
    return TA_COLLISION_TRANSPARENT;
}

void TA_Bee::load(TA_Point position, TA_BeeHive* hive) {
    loadFromToml("objects/beehive.toml");
    setAnimation("bee");

    hitbox.setRectangle(TA_Point(4, 4), TA_Point(12, 12));
    this->position = position;
    this->hive = hive;
    updatePosition();

    flyDownY = position.y + 48;
    direction = (TA::random::next() % 2 == 0);
    leftCircle = !direction;
}

bool TA_Bee::update() {
    if(flyDown) {
        updateFlyDown();
    } else {
        updateNormal();
    }
    updatePosition();

    if((objectSet->checkCollision(hitbox) & (TA_COLLISION_ATTACK | TA_COLLISION_NAPALM)) != 0) {
        objectSet->spawnObject<TA_Explosion>(position, 0, TA_EXPLOSION_NEUTRAL);
        hive->deleteBee();
        return false;
    }
    return true;
}

void TA_Bee::updateFlyDown() {
    position.y += flySpeed * TA::elapsedTime;
    if(position.y > flyDownY) {
        position.y = flyDownY;
        flyDown = false;
        leftCircleCenter = position - TA_Point(circleRadius, 0);
        rightCircleCenter = position + TA_Point(circleRadius, 0);
    }
}

void TA_Bee::updateNormal() {
    static constexpr float circleTime = 2 * TA::pi * circleRadius / flySpeed;

    timer += TA::elapsedTime;
    if(timer > circleTime) {
        timer -= circleTime;
        leftCircle = !leftCircle;
        direction = !direction;
    }

    TA_Point circleCenter = (leftCircle ? leftCircleCenter : rightCircleCenter);
    float circlePos = 2 * TA::pi * timer / circleTime;
    if(!leftCircle) {
        circlePos += TA::pi;
        if(circlePos > TA::pi * 2) {
            circlePos -= TA::pi * 2;
        }
    }
    if(direction) {
        circlePos = 2 * TA::pi - circlePos;
    }

    TA_Point newPosition = circleCenter + TA_Point(std::cos(circlePos), std::sin(circlePos)) * circleRadius;
    setFlip(newPosition.x > position.x);
    position = newPosition;
}

void TA_StrongBee::load(TA_Point position) {
    loadFromToml("objects/beehive.toml");
    setAnimation("strong_bee");

    hitbox.setRectangle(TA_Point(4, 4), TA_Point(12, 12));
    this->position = position;
    updatePosition();
    angle = getNeedeedAngle();
}

bool TA_StrongBee::update() {
    if(timer < idleTime) {
        timer += TA::elapsedTime;
        return true;
    }

    float neededAngle = getNeedeedAngle();
    float leftDist = std::fmod(angle - neededAngle + TA::pi * 2, TA::pi * 2);
    float rightDist = std::fmod(neededAngle - angle + TA::pi * 2, TA::pi * 2);
    angle += rotateSpeed * (leftDist < rightDist ? -1 : 1) * TA::elapsedTime;
    angle = std::fmod(angle + TA::pi * 2, TA::pi * 2);
    position = position + TA_Point(std::cos(angle), std::sin(angle)) * speed;
    updatePosition();

    if((objectSet->checkCollision(hitbox) & (TA_COLLISION_ATTACK | TA_COLLISION_NAPALM)) != 0) {
        objectSet->spawnObject<TA_Explosion>(position, 0, TA_EXPLOSION_NEUTRAL);
        return false;
    }
    return true;
}

float TA_StrongBee::getNeedeedAngle() {
    TA_Point vector = objectSet->getCharacterPosition() - (position + TA_Point(8, 8));
    float length = vector.length();
    vector.x /= length;
    vector.y /= length;
    if(vector.y >= 0) {
        return std::acos(vector.x);
    }
    return TA::pi * 2 - std::acos(vector.x);
}
