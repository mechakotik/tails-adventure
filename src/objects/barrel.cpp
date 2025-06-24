#include "barrel.h"
#include "explosion.h"
#include "splash.h"

void TA_Barrel::load(TA_Point position, TA_Point velocity) {
    loadFromToml("objects/cruiser/barrel.toml");
    this->position = position;
    this->velocity = velocity;
    hitbox.setRectangle({0, 0}, {8, 8});
    collisionType = TA_COLLISION_DAMAGE;

    updatePosition();
}

bool TA_Barrel::update() {
    static constexpr float gravity = 0.125;
    static constexpr float drag = 0.015;
    static constexpr float maxUnderwaterYsp = 1;
    static constexpr float minXsp = 0.5;

    float waterLevel = objectSet->getWaterLevel();
    velocity.y += gravity * TA::elapsedTime;
    velocity.x -= drag * TA::elapsedTime;
    if(position.y >= waterLevel) {
        velocity.y = std::min(velocity.y, maxUnderwaterYsp);
    }
    velocity.x = std::max(velocity.x, minXsp);

    if(position.y + 8 < waterLevel && position.y + 8 + velocity.y * TA::elapsedTime >= waterLevel) {
        objectSet->spawnObject<TA_Splash>(position - TA_Point(4, 4));
    }

    position = position + velocity * TA::elapsedTime;
    updatePosition();

    if(position.y > 232 || (objectSet->checkCollision(hitbox) & TA_COLLISION_CHARACTER) != 0) {
        objectSet->spawnObject<TA_Explosion>(position - TA_Point(4, 4), 0, TA_EXPLOSION_ENEMY);
        return false;
    }
    return true;
}
