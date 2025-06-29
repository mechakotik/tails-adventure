#include "explosion.h"
#include "error.h"
#include "tools.h"

void TA_Explosion::load(TA_Point position, int newDelay, TA_ExplosionType type) {
    loadFromToml("objects/explosion.toml");
    setPosition(position);
    setAnimation("explosion");

    delay = newDelay;
    hitbox.setRectangle(TA_Point(-2, -2), TA_Point(17, 17));
    hitbox.setPosition(position);

    collisionType = TA_COLLISION_TRANSPARENT;
    if(type == TA_EXPLOSION_CHARACTER && delay == 0) {
        collisionType |= TA_COLLISION_ATTACK;
    } else if(type == TA_EXPLOSION_ENEMY) {
        collisionType = TA_COLLISION_DAMAGE;
    }
}

bool TA_Explosion::update() {
    timer += TA::elapsedTime;
    if(!isAnimated()) {
        return false;
    }
    return true;
}

void TA_Explosion::draw() {
    if(timer >= delay) {
        TA_Sprite::draw();
    }
}
