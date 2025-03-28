#include "napalm_fire.h"
#include <iostream>

void TA_NapalmFire::load(TA_Point position, float xsp) {
    this->position = position;
    velocity = {xsp, 0};

    loadFromToml("objects/napalm_fire.toml");
    setAnimation("fire");

    TA_Rect explosionHitbox;
    explosionHitbox.setRectangle(TA_Point(0, 0), TA_Point(8, 28));
    hitboxVector.push_back({explosionHitbox, TA_COLLISION_ATTACK});

    TA_Rect napalmHitbox;
    napalmHitbox.setRectangle(TA_Point(-4, -64), TA_Point(12, 28));
    hitboxVector.push_back({napalmHitbox, TA_COLLISION_NAPALM});

    updatePosition();
}

bool TA_NapalmFire::checkPawnCollision(TA_Rect& hitbox) {
    int flags = objectSet->checkCollision(hitbox);
    if(flags & (TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP | TA_COLLISION_PUSHABLE)) {
        return true;
    }
    return false;
}

bool TA_NapalmFire::update() {
    int flags = moveAndCollide(topLeft, bottomRight, velocity * TA::elapsedTime, true);
    if(flags & TA_WALL_COLLISION) {
        velocity.x = 0;
    }

    updatePosition();

    if(!TA_Sprite::isAnimated()) {
        return false;
    }
    return true;
}
