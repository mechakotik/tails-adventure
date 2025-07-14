#include "sniper.h"
#include "bullet.h"
#include "dead_kukku.h"

void TA_Sniper::load(TA_Point position) {
    loadFromToml("objects/sniper.toml");
    this->position = position;
    collisionType = TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;
    hitbox.setRectangle({9, 17}, {24, 32});
    updatePosition();
    setAnimation("crouch");
}

bool TA_Sniper::update() {
    if(aim) {
        updateAim();
        if((objectSet->checkCollision(hitbox) & TA_COLLISION_ATTACK) != 0) {
            objectSet->spawnObject<TA_DeadKukku>(position + TA_Point(8, 0));
            return false;
        }
    } else {
        updateCrouch();
    }

    return true;
}

void TA_Sniper::updateAim() {
    float newTimer = timer + TA::elapsedTime;
    if(timer < fireTime && newTimer >= fireTime) {
        objectSet->spawnObject<TA_SniperBullet>(
            position + TA_Point((flip ? 27 : -1), 16), TA_Point((flip ? 1.5 : -1.5), 0));
    }
    timer = newTimer;

    if(!isAnimated()) {
        aim = false;
        setAnimation("crouch");
        hitbox.setRectangle({9, 17}, {24, 32});
        updateFlip();
    }
}

void TA_Sniper::updateCrouch() {
    TA_Point delta = objectSet->getCharacterPosition() - (position + TA_Point(16, 16));
    if(!isAnimated() && abs(delta.x) <= 96 && abs(delta.y) <= 64) {
        aim = true;
        timer = 0;
        setAnimation("aim");
        hitbox.setRectangle({9, 2}, {24, 32});
        updateFlip();
    }
}

void TA_Sniper::updateFlip() {
    float deltaX = objectSet->getCharacterPosition().x - (position.x + 16);
    flip = (deltaX > 0);
    setFlip(flip);
}
