#include "bullet.h"
#include "explosion.h"
#include "tilemap.h"
#include "tools.h"

void TA_Bullet::load(std::string filename, TA_Point newPosition, TA_Point newVelocity) {
    loadFromToml(filename);
    position = newPosition;
    velocity = newVelocity;
    hitbox.setRectangle(TA_Point(0, 0), TA_Point(getWidth(), getHeight()));
    collisionType = TA_COLLISION_DAMAGE;
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
    collisionType = TA_COLLISION_ATTACK;
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

void TA_SniperBullet::load(TA_Point position, TA_Point velocity) {
    TA_Bullet::load("objects/sniper_bullet.toml", position, velocity);
}

void TA_SniperBullet::onDestroy() {
    if((objectSet->checkCollision(hitbox) & TA_COLLISION_CHARACTER) != 0) {
        objectSet->spawnObject<TA_Explosion>(position - TA_Point(5, 6), 0, TA_EXPLOSION_ENEMY);
    }
}

void TA_HeavyGunBullet::load(TA_Point position, TA_Point velocity) {
    TA_Bullet::load("objects/heavy_gun_bullet.toml", position, velocity);
}

void TA_HeavyGunBullet::onDestroy() {
    if((objectSet->checkCollision(hitbox) & TA_COLLISION_CHARACTER) != 0) {
        objectSet->spawnObject<TA_Explosion>(position - TA_Point(5, 6), 0, TA_EXPLOSION_ENEMY);
    }
}

void TA_PilotBullet::load(TA_Point position, TA_Point velocity) {
    TA_Bullet::load("objects/pilot_bullet.toml", position, velocity);
    setAnimation("bullet");
}

void TA_PilotBullet::onDestroy() {
    if((objectSet->checkCollision(hitbox) & TA_COLLISION_CHARACTER) != 0) {
        objectSet->spawnObject<TA_Explosion>(position - TA_Point(4, 4), 0, TA_EXPLOSION_ENEMY);
    }
}

void TA_DrFukurokovLazer::load(TA_Point position, TA_Point velocity) {
    TA_Bullet::load("objects/dr_fukurokov/lazer.toml", position, velocity);
}

void TA_DrFukurokovLazer::onDestroy() {
    objectSet->spawnObject<TA_Explosion>(position - TA_Point(7, 0), 0, TA_EXPLOSION_ENEMY);
}

void TA_MechaGolemBullet::load(TA_Point position, TA_Point velocity) {
    TA_Bullet::load("objects/mecha_golem/bullet.toml", position, velocity);
    setAnimation("bullet");
    explosionSound.load("sound/explosion_small.ogg", TA_SOUND_CHANNEL_SFX3);
}

void TA_MechaGolemBullet::onDestroy() {
    objectSet->spawnObject<TA_Explosion>(position - TA_Point(7, 0), 0, TA_EXPLOSION_ENEMY);
    explosionSound.play();
}
