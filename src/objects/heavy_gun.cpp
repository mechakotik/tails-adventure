#include "heavy_gun.h"
#include "bullet.h"

void TA_HeavyGun::load(TA_Point position, bool flip) {
    loadFromToml("objects/heavy_gun.toml");
    this->position = position;
    this->flip = flip;
    setFlip(flip);
    hitbox.setRectangle({1, 7}, {31, 15});
    collisionType = TA_COLLISION_SOLID;
    updatePosition();
}

bool TA_HeavyGun::update() {
    if(timer < cooldown) {
        timer += TA::elapsedTime;
    }
    if(!objectSet->isVisible(TA_Rect(position - TA_Point(16, 16), position + TA_Point(32, 32)))) {
        return true;
    }
    if(timer >= cooldown) {
        objectSet->spawnObject<TA_HeavyGunBullet>(
            position + TA_Point((flip ? -5 : 31), 6), TA_Point((flip ? -1.5 : 1.5), 0));
        timer = 0;
    }
    return true;
}
