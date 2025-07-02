#include "underwater_barrier.h"
#include "particle.h"
#include "save.h"
#include "tools.h"

void TA_UnderwaterBarrier::load(TA_Point position, std::string particlePath) {
    if(TA::save::getSaveParameter("underwater_barrier_passed") == 1) {
        passed = true;
        return;
    }

    loadFromToml("objects/underwater_barrier.toml");
    this->position = position;
    this->particlePath = particlePath;
    hitbox.setRectangle({0, 0}, {48, 32});
    collisionType = TA_COLLISION_SOLID;
    updatePosition();
}

bool TA_UnderwaterBarrier::update() {
    if(passed) {
        return false;
    }
    if(timer < cooldown) {
        timer += TA::elapsedTime;
        return true;
    }

    if((objectSet->checkCollision(hitbox) & TA_COLLISION_ATTACK) != 0) {
        hp--;
        timer = 0;
        if(hp <= 0) {
            for(int i = 0; i < 4; i++) {
                objectSet->spawnObject<TA_Particle>(
                    particlePath, position + TA_Point(i * 16 + 2, 8), TA_Point(-0.5, -2), TA_Point(0, gravity));
                objectSet->spawnObject<TA_Particle>(
                    particlePath, position + TA_Point(i * 16 + 8, 8), TA_Point(0.5, -2), TA_Point(0, gravity));
                objectSet->spawnObject<TA_Particle>(
                    particlePath, position + TA_Point(i * 16 + 2, 18), TA_Point(-0.5, -0.5), TA_Point(0, gravity));
                objectSet->spawnObject<TA_Particle>(
                    particlePath, position + TA_Point(i * 16 + 8, 18), TA_Point(0.5, -0.5), TA_Point(0, gravity));
            }
            TA::save::setSaveParameter("underwater_barrier_passed", 1);
            return false;
        }
    }

    return true;
}
