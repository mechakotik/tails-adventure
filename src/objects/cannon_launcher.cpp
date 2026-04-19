#include "cannon_launcher.h"
#include "bullet.h"
#include "dead_kukku.h"
#include "defs.h"
#include "tools.h"

void TA_CannonLauncher::load(TA_Point position, bool flip) {
    loadFromToml("objects/cannon_launcher.toml");
    hitbox.setRectangle(TA_Point(8, 3), TA_Point(32, 22));
    collisionType = TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;
    this->position = position;
    this->flip = flip;
    updatePosition();
    setFlip(flip);
}

bool TA_CannonLauncher::update() {
    static constexpr float cooldown = 80;

    if(objectSet->isVisible(hitbox)) {
        if(!lookUp && (objectSet->getCharacterPosition().y < position.y - 16)) {
            lookUp = true;
        } else if(lookUp && (objectSet->getCharacterPosition().y > position.y - 8)) {
            lookUp = false;
        }
        setAnimation(lookUp ? "look_up" : "look_forward");
        if(timer < cooldown) {
            timer += TA::elapsedTime;
        } else {
            if(lookUp) {
                objectSet->spawnObject<TA_PilotBullet>(
                    position + TA_Point(flip ? 26 : -2, 1), TA_Point(flip ? 2 : -1.4, -1.4));
            } else {
                objectSet->spawnObject<TA_PilotBullet>(
                    position + TA_Point(flip ? 28 : -4, 11), TA_Point(flip ? 2 : -2, 0));
            }
            timer = 0;
        }
    }

    if((objectSet->checkCollision(hitbox) & TA_COLLISION_ATTACK) != 0) {
        objectSet->spawnObject<TA_DeadKukku>(position + TA_Point(flip ? 0 : 8, 0));
        return false;
    }

    return true;
}
