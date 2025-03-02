#include "ring.h"
#include "error.h"
#include "pawn.h"
#include "save.h"
#include "tilemap.h"
#include "tools.h"

void TA_Ring::load(TA_Point position, TA_Point velocity, double delay) {
    this->position = position;
    this->velocity = velocity;
    this->delay = delay;

    loadFromToml("objects/ring.toml");
    setAnimation("ring");
    hitbox.setRectangle({0, 0}, {7, 7});
    ringSound.load("sound/ring.ogg", TA_SOUND_CHANNEL_SFX2);
    setPosition(position);
}

void TA_Ring::load(TA_Point position, double speed) {
    load(position, {0, speed}, 0);
}

void TA_Ring::loadStationary(TA_Point position) {
    load(position, {0, 0}, 0);
    stationary = true;
}

bool TA_Ring::checkPawnCollision(TA_Polygon& hitbox) {
    int flags;
    objectSet->checkCollision(hitbox, flags);
    return flags & (TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP);
}

bool TA_Ring::update() {
    if(collected) {
        return isAnimated();
    }

    if(!stationary) {
        double currentGrv = (TA::save::getSaveParameter("seafox") == 1 ? waterGrv : grv);
        velocity.y += currentGrv * TA::elapsedTime;
        TA_Point topLeft{0, 0}, bottomRight{8, 8};
        int flags = moveAndCollide(topLeft, bottomRight, velocity * TA::elapsedTime);
        setPosition(position);

        if(velocity.x > 0) {
            velocity.x = std::max(0.0, velocity.x - drag * TA::elapsedTime);
        } else {
            velocity.x = std::min(0.0, velocity.x + drag * TA::elapsedTime);
        }

        if((flags & TA_GROUND_COLLISION) != 0 && velocity.y > 0) {
            velocity.y *= -slowdown;
            if(velocity.y > -0.5) {
                velocity.y = 0;
            }
        }
        if((flags & TA_WALL_COLLISION) != 0) {
            velocity.x *= -1;
        }
        if((flags & TA_CEIL_COLLISION) != 0 && velocity.y < 0) {
            velocity.y *= -1;
        }
    }

    if(timer > delay) {
        hitbox.setPosition(position);
        if((objectSet->checkCollision(hitbox) & TA_COLLISION_CHARACTER) != 0) {
            ringSound.play();
            objectSet->addRings(1);
            collected = true;
            timer = 0;
            setAnimation("flick");
            return true;
        }
    }

    timer += TA::elapsedTime;
    if(!stationary && timer > maxTime) {
        return false;
    }
    return true;
}
