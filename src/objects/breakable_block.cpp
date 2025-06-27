#include "breakable_block.h"
#include "particle.h"
#include "ring.h"

void TA_BreakableBlock::load(
    std::string path, std::string particlePath, TA_Point position, bool dropsRing, bool strong) {
    TA_Sprite::load(path);
    this->particlePath = particlePath;
    this->position = position;
    this->dropsRing = dropsRing;
    this->strong = strong;
    setPosition(position);
    hitbox.setRectangle(TA_Point(0, 0), TA_Point(getWidth(), getHeight()));
    hitbox.setPosition(position);
    collisionType = TA_COLLISION_SOLID;
    breakSound.load("sound/break.ogg", TA_SOUND_CHANNEL_SFX2);
}

bool TA_BreakableBlock::update() {
    int flags;
    objectSet->checkCollision(hitbox, flags);

    bool shouldBreak = false;
    if((flags & TA_COLLISION_NAPALM) == 0) {
        if(objectSet->getLinks().character != nullptr) {
            if(strong && (flags & TA_COLLISION_ATTACK_STRONG) != 0) {
                shouldBreak = true;
            } else if(!strong && (flags & TA_COLLISION_ATTACK) != 0) {
                shouldBreak = true;
            }
        } else if(objectSet->getLinks().seaFox != nullptr && (flags & TA_COLLISION_DRILL) != 0) {
            shouldBreak = true;
            breakSound.play();
        }
    }

    if(shouldBreak) { // TODO: particles positions should depend on block size
        objectSet->spawnObject<TA_Particle>(
            particlePath, position + TA_Point(2, 2), TA_Point(-0.5, -2), TA_Point(0, grv));
        objectSet->spawnObject<TA_Particle>(
            particlePath, position + TA_Point(8, 2), TA_Point(0.5, -2), TA_Point(0, grv));
        objectSet->spawnObject<TA_Particle>(
            particlePath, position + TA_Point(2, 8), TA_Point(-0.5, -0.5), TA_Point(0, grv));
        objectSet->spawnObject<TA_Particle>(
            particlePath, position + TA_Point(8, 8), TA_Point(0.5, -0.5), TA_Point(0, grv));
        objectSet->resetInstaShield();
        if(dropsRing) {
            objectSet->spawnObject<TA_Ring>(position + TA_Point(4, 4));
        }
        return false;
    }
    return true;
}
