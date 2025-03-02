#include "bat_robot.h"
#include "explosion.h"
#include "ring.h"
#include "tools.h"

void TA_BatRobot::load(TA_Point newPosition) {
    loadFromToml("objects/bat_robot.toml");
    setAnimation("idle");

    position = newPosition;
    hitbox.setRectangle(TA_Point(5, 0), TA_Point(18, 14));
    updatePosition();
}

bool TA_BatRobot::checkPawnCollision(TA_Polygon& collisionHitbox) {
    int flags = objectSet->checkCollision(collisionHitbox);
    return flags & TA_COLLISION_SOLID;
}

bool TA_BatRobot::update() {
    switch(state) {
        case STATE_IDLE: {
            setAnimation("idle");
            TA_Point characterPosition = objectSet->getCharacterPosition();
            double centeredX = position.x + 12;
            if(abs(characterPosition.x - centeredX) <= 80 && characterPosition.y - position.y <= 80) {
                state = STATE_ACTIVE;
            }
            break;
        }

        case STATE_ACTIVE: {
            setAnimation("active");
            TA_Point characterPosition = objectSet->getCharacterPosition();
            timer += TA::elapsedTime;
            double centeredX = position.x + 12;
            double deltaY = characterPosition.y - position.y - 8;
            if(timer >= cooldownTime && 0 <= deltaY && deltaY <= 80 && abs(characterPosition.x - centeredX) <= 64) {
                position.y += 0.1;
                velocity = {TA::sign(characterPosition.x - position.x) * 0.5, findOptimalSpeed(deltaY)};
                state = STATE_ATTACK;
            } else if(abs(characterPosition.x - centeredX) >= 82 || characterPosition.y - position.y >= 82) {
                state = STATE_IDLE;
            }
            break;
        }

        case STATE_ATTACK: {
            setAnimation("attack");
            velocity.y += gravity * TA::elapsedTime;
            int flags = moveAndCollide(TA_Point(5, 0), TA_Point(18, 14), velocity * TA::elapsedTime);
            if(flags & TA_GROUND_COLLISION) {
                velocity.y = std::min(double(0), velocity.y);
            }
            if(flags & TA_CEIL_COLLISION) {
                state = STATE_ACTIVE;
                timer = 0;
            }
            break;
        }
    }

    updatePosition();

    if(objectSet->checkCollision(hitbox) & TA_COLLISION_ATTACK) {
        objectSet->spawnObject<TA_Explosion>(position + TA_Point(4, 0), 0, TA_EXPLOSION_NEUTRAL);
        objectSet->resetInstaShield();
        if(objectSet->enemyShouldDropRing()) {
            objectSet->spawnObject<TA_Ring>(position + TA_Point(8, 4));
        }
        return false;
    }
    return true;
}

double TA_BatRobot::findOptimalSpeed(double deltaY) {
    auto distance = [&](double jumpSpeed) {
        double t = -(jumpSpeed - gravity / 2) / gravity;
        return t * jumpSpeed + t * (t - 1) / 2 * gravity;
    };
    double left = 1 * speed, right = 3.2 * speed, eps = 1e-3;
    while(right - left > eps) {
        double mid = (left + right) / 2;
        if(distance(mid) < deltaY) {
            left = mid;
        } else {
            right = mid;
        }
    }
    return left;
}
