#include "explosion.h"
#include "error.h"
#include "tools.h"

void TA_Explosion::load(TA_Point position, int delay, TA_ExplosionType type, TA_Point velocity) {
    loadFromToml("objects/explosion.toml");
    setAnimation("explosion");

    this->position = position;
    this->delay = delay;
    this->velocity = velocity;
    hitbox.setRectangle(TA_Point(-2, -2), TA_Point(17, 17));
    updatePosition();

    collisionType = TA_COLLISION_TRANSPARENT;
    if(type == TA_EXPLOSION_CHARACTER) {
        collisionType |= TA_COLLISION_ATTACK;
    } else if(type == TA_EXPLOSION_ENEMY) {
        collisionType |= TA_COLLISION_DAMAGE;
    } else if(type == TA_EXPLOSION_LARGE) {
        collisionType |= TA_COLLISION_ATTACK | TA_COLLISION_ATTACK_LARGE;
    }
}

bool TA_Explosion::update() {
    timer += TA::elapsedTime;
    if(timer >= delay) {
        position += velocity * TA::elapsedTime;
    }
    updatePosition();
    return isAnimated();
}

void TA_Explosion::draw() {
    if(timer >= delay) {
        TA_Sprite::draw();
    }
}
