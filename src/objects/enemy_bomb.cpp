#include "enemy_bomb.h"
#include "defs.h"
#include "explosion.h"
#include "tools.h"

void TA_EnemyBomb::load(
    const std::filesystem::path& path, TA_Point position, TA_Point topLeft, TA_Point bottomRight, TA_Point velocity) {
    this->position = position;
    this->topLeft = topLeft;
    this->bottomRight = bottomRight;
    this->velocity = velocity;

    TA_Sprite::loadFromToml(path);
    setAnimation("idle");
    hitbox.setRectangle(topLeft - TA_Point(0.5, 0.5), bottomRight + TA_Point(0.5, 0.5));
    collisionType = TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;
    updatePosition();
}

bool TA_EnemyBomb::update() {
    velocity.y += grv * TA::elapsedTime;
    auto [delta, flags] = objectSet->moveAndCollide(position, topLeft, bottomRight, velocity * TA::elapsedTime,
        TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP | TA_COLLISION_CHARACTER);
    position += delta;

    if(flags != 0) {
        objectSet->spawnObject<TA_Explosion>(position - TA_Point(4, 1), 0, TA_EXPLOSION_ENEMY);
        return false;
    }

    updatePosition();

    flags = objectSet->checkCollision(hitbox);
    if((flags & TA_GENERIC_ATTACK) != 0) {
        objectSet->spawnObject<TA_Explosion>(position - TA_Point(4, 1), 0, TA_EXPLOSION_NEUTRAL);
        return false;
    }

    return true;
}
