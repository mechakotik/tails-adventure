#include "napalm_fire.h"

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

bool TA_NapalmFire::update() {
    auto [delta, flags] = objectSet->moveAndCollide(position, topLeft, bottomRight, velocity * TA::elapsedTime,
        TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP | TA_COLLISION_PUSHABLE, true);
    position += delta;
    if((flags & TA_WALL_COLLISION) != 0) {
        velocity.x = 0;
    }

    updatePosition();
    return TA_Sprite::isAnimated();
}
