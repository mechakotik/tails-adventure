#include "dead_kukku.h"
#include "explosion.h"
#include "ring.h"
#include "tools.h"

void TA_DeadKukku::load(TA_Point newPosition) {
    loadFromToml("objects/pf_enemies.toml");
    setAnimation("death");
    position = newPosition;
    objectSet->spawnObject<TA_Explosion>(
        position + TA_Point(double(TA::random::next() % 16) - 4, double(TA::random::next() % 16) - 8), 0,
        TA_EXPLOSION_NEUTRAL);
    if(objectSet->enemyShouldDropRing()) {
        objectSet->spawnObject<TA_Ring>(position + TA_Point(8, 24), -2.5);
    }
}

bool TA_DeadKukku::update() {
    velocity.y += grv * TA::elapsedTime;
    position = position + velocity * TA::elapsedTime;
    setPosition(position);
    timer += TA::elapsedTime;
    if(timer > deathTime) {
        return false;
    }
    return true;
}
