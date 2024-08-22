#include "breakable_block.h"
#include "particle.h"
#include "ring.h"

void TA_BreakableBlock::load(std::string path, std::string newParticlePath, TA_Point newPosition, bool newDropsRing)
{
    TA_Sprite::load(path);
    particlePath = newParticlePath;
    position = newPosition;
    dropsRing = newDropsRing;
    setPosition(position);
    hitbox.setRectangle(TA_Point(0, 0), TA_Point(getWidth(), getHeight()));
    hitbox.setPosition(position);
    breakSound.load("sound/break.ogg", TA_SOUND_CHANNEL_SFX2);
}

bool TA_BreakableBlock::update()
{
    int flags;
    objectSet->checkCollision(hitbox, flags);

    bool shouldBreak = false;
    if(objectSet->getLinks().character && flags & (TA_COLLISION_EXPLOSION | TA_COLLISION_HAMMER)) {
        shouldBreak = true;
    }
    else if(objectSet->getLinks().seaFox && flags & (TA_COLLISION_DRILL)) {
        shouldBreak = true;
        breakSound.play();
    }

    if(shouldBreak) { // TODO: particles positions should depend on block size
        objectSet->spawnObject<TA_Particle>(particlePath, position + TA_Point(2, 2), TA_Point(-0.5, -2), TA_Point(0, grv));
        objectSet->spawnObject<TA_Particle>(particlePath, position + TA_Point(8, 2), TA_Point(0.5, -2), TA_Point(0, grv));
        objectSet->spawnObject<TA_Particle>(particlePath, position + TA_Point(2, 8), TA_Point(-0.5, -0.5), TA_Point(0, grv));
        objectSet->spawnObject<TA_Particle>(particlePath, position + TA_Point(8, 8), TA_Point(0.5, -0.5), TA_Point(0, grv));
        objectSet->resetInstaShield();
        if(dropsRing) {
            objectSet->spawnObject<TA_Ring>(position + TA_Point(4, 4));
        }
        return false;
    }
    return true;
}
