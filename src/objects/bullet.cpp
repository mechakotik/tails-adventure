#include "bullet.h"
#include "explosion.h"
#include "tools.h"

void TA_Bullet::load(std::string filename, TA_Point newPosition, TA_Point newVelocity) {
    loadFromToml(filename);
    position = newPosition;
    velocity = newVelocity;
    hitbox.setRectangle(TA_Point(0, 0), TA_Point(getWidth(), getHeight()));
    updatePosition();
}

bool TA_Bullet::update() {
    position = position + velocity * TA::elapsedTime;
    updatePosition();

    int flags = objectSet->checkCollision(hitbox);
    if(flags & getCollisionFlags()) {
        onDestroy();
        return false;
    }
    return true;
}

void TA_BirdWalkerBullet::load(TA_Point newPosition, TA_Point newVelocity) {
    TA_Bullet::load("objects/bird_walker/bullet.toml", newPosition, newVelocity);
    setAnimation("bullet");
}

bool TA_BirdWalkerBullet::update() {
    switch(getAnimationFrame()) {
        case 0:
            hitbox.setRectangle(TA_Point(6, 9), TA_Point(9, 14));
            break;

        case 1:
            hitbox.setRectangle(TA_Point(5, 5), TA_Point(11, 13));
            break;

        case 2:
            hitbox.setRectangle(TA_Point(3, 5), TA_Point(14, 15));
            break;

        default:
            hitbox.setRectangle(TA_Point(1, 3), TA_Point(15, 15));
            break;
    }
    return TA_Bullet::update();
}

void TA_VulcanGunBullet::load(TA_Point position, TA_Point velocity) {
    TA_Bullet::load("objects/vulcan_gun_bullet.toml", position, velocity);
    setAnimation("bullet");
    explosionSound.load("sound/explosion_small.ogg", TA_SOUND_CHANNEL_SFX2);
}

bool TA_VulcanGunBullet::update() {
    timer += TA::elapsedTime;
    if(timer > existTime) {
        return false;
    }
    return TA_Bullet::update();
}

void TA_VulcanGunBullet::onDestroy() {
    objectSet->spawnObject<TA_Explosion>(position - TA_Point(5, 5), 0, TA_EXPLOSION_CHARACTER);
    explosionSound.play();
}
