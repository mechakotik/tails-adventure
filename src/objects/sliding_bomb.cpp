#include "sliding_bomb.h"
#include "explosion.h"
#include "object_set.h"
#include "tools.h"

void TA_SlidingBomb::load(TA_Point position, bool flip, bool* lock) {
    loadFromToml("objects/sliding_bomb.toml");
    setAnimation("idle");
    this->position = position;
    this->flip = flip;
    this->lock = lock;
    hitbox.setRectangle({3, 5}, {13, 16});
    velocity = {flip ? -1 : 1, 0};
    updatePosition();
}

bool TA_SlidingBomb::update() {
    auto [delta, flags] =
        objectSet->moveAndCollide(position, {3, 5}, {13, 16}, velocity * TA::elapsedTime, TA_COLLISION_SOLID, true);
    position += delta;
    updatePosition();

    if((flags & TA_GROUND_COLLISION) != 0) {
        velocity.x = (flip ? -1.0F : 1.0F);
        velocity.y = 0;
    } else {
        velocity.y += gravity * TA::elapsedTime;
    }
    if((flags & TA_WALL_COLLISION) != 0) {
        velocity.x = 0;
    }

    bool destroy = false;
    destroy |= (TA::equal(velocity.x, 0) && TA::equal(velocity.y, 0));
    destroy |= (objectSet->checkCollision(hitbox) & TA_COLLISION_CHARACTER) != 0;

    if(destroy) {
        objectSet->spawnObject<TA_Explosion>(position, 0, TA_EXPLOSION_ENEMY);
        *lock = false;
        return false;
    }
    return true;
}

void TA_SlidingBombSpawner::load(TA_Point position, bool flip) {
    this->position = position;
    this->flip = flip;
}

bool TA_SlidingBombSpawner::update() {
    if(lock || !objectSet->isVisible(TA_Rect(position - TA_Point(16, 16), position + TA_Point(32, 32)))) {
        return true;
    }

    timer += TA::elapsedTime;
    if(timer > cooldown) {
        objectSet->spawnObject<TA_SlidingBomb>(position - TA_Point(0, 1), flip, &lock);
        lock = true;
        timer = 0;
    }

    return true;
}
