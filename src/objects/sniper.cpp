#include "sniper.h"

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
    } else {
        updateCrouch();
    }

    return true;
}

void TA_Sniper::updateAim() {
    float newTimer = timer + TA::elapsedTime;
    if(timer < fireTime && newTimer >= fireTime) {
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
    if(!isAnimated() && abs(delta.x) <= 192 && abs(delta.y) <= 80) {
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
