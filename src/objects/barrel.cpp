#include "barrel.h"
#include <bomb.h>
#include "explosion.h"

void TA_Barrel::load(TA_Point position, TA_Point velocity) {
    loadFromToml("objects/cruiser/barrel.toml");
    this->position = position;
    this->velocity = velocity;
    hitbox.setRectangle({0, 0}, {8, 8});
    updatePosition();
}

bool TA_Barrel::update() {
    static constexpr float gravity = 0.125;
    static constexpr float drag = 0.015;
    static constexpr float maxUnderwaterYsp = 1;
    static constexpr float minXsp = 0.5;

    // TODO: don't hardcode water level
    velocity.y += gravity * TA::elapsedTime;
    velocity.x -= drag * TA::elapsedTime;
    if(position.y >= 140) {
        velocity.y = std::min(velocity.y, maxUnderwaterYsp);
    }
    velocity.x = std::max(velocity.x, minXsp);

    position = position + velocity * TA::elapsedTime;
    updatePosition();

    if(objectSet->checkCollision(hitbox) & (TA_COLLISION_SOLID | TA_COLLISION_CHARACTER)) {
        objectSet->spawnObject<TA_Explosion>(position - TA_Point(4, 4), 0, TA_EXPLOSION_ENEMY);
        return false;
    }
    return true;
}
