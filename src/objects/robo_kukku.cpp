#include "robo_kukku.h"
#include "dead_kukku.h"
#include "explosion.h"
#include "object_set.h"
#include "tilemap.h"
#include "tools.h"

void TA_RoboKukku::load(TA_Point position, bool flip, bool* lock) {
    loadFromToml("objects/robo_kukku.toml");
    setAnimation("idle");
    setFlip(!flip);
    this->position = position;
    this->flip = flip;
    this->lock = lock;
    hitbox.setRectangle({3, 5}, {13, 32});
    velocity = {flip ? -1 : 1, 0};
    updatePosition();
}

bool TA_RoboKukku::update() {
    auto [delta, flags] =
        objectSet->moveAndCollide(position, {3, 5}, {13, 32}, velocity * TA::elapsedTime, TA_COLLISION_SOLID, true);
    position += delta;
    updatePosition();

    if((flags & TA_GROUND_COLLISION) != 0) {
        velocity.y = 0;
    } else {
        velocity.x = 0;
        velocity.y += gravity * TA::elapsedTime;
    }

    int collisionFlags = objectSet->checkCollision(hitbox);
    if(TA::equal(velocity.y, 0)) {
        if((collisionFlags & TA_COLLISION_CONVEYOR_BELT_LEFT) != 0) {
            setFlip(false);
            velocity.x = -1;
        }
        if((collisionFlags & TA_COLLISION_CONVEYOR_BELT_RIGHT) != 0) {
            setFlip(true);
            velocity.x = 1;
        }
    }

    bool destroy = false;
    destroy |= (TA::equal(velocity.x, 0) && TA::equal(velocity.y, 0));
    destroy |= (collisionFlags & TA_COLLISION_CHARACTER) != 0;
    destroy |= (TA::equal(velocity.y, 0) &&
                (collisionFlags & (TA_COLLISION_CONVEYOR_BELT_LEFT | TA_COLLISION_CONVEYOR_BELT_RIGHT)) == 0);

    if(destroy) {
        objectSet->spawnObject<TA_DeadKukku>(position);
        objectSet->spawnObject<TA_Explosion>(position + TA_Point(0, 16), 0, TA_EXPLOSION_ENEMY);
        *lock = false;
        return false;
    }
    return true;
}

void TA_RoboKukkuSpawner::load(TA_Point position, bool flip) {
    this->position = position;
    this->flip = flip;
}

bool TA_RoboKukkuSpawner::update() {
    if(lock || !objectSet->isVisible(TA_Rect(position - TA_Point(16, 16), position + TA_Point(32, 48)))) {
        return true;
    }

    timer += TA::elapsedTime;
    if(timer > cooldown) {
        objectSet->spawnObject<TA_RoboKukku>(position - TA_Point(0, 1), flip, &lock);
        lock = true;
        timer = 0;
    }

    return true;
}
