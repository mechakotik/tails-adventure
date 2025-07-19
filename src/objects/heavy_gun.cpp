#include "heavy_gun.h"
#include "bullet.h"

void TA_HeavyGun::load(TA_Point position) {
    loadFromToml("objects/heavy_gun.toml");
    this->position = position;
    hitbox.setRectangle({0, 7}, {31, 15});
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
        objectSet->spawnObject<TA_HeavyGunBullet>(position + TA_Point(31, 6), TA_Point(1.5, 0));
        timer = 0;
    }
    return true;
}
