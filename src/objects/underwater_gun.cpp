#include "underwater_gun.h"
#include "explosion.h"
#include "tools.h"

void TA_UnderwaterGun::load(TA_Point position, bool flip) {
    loadFromToml("objects/underwater_gun.toml");
    this->position = position;
    this->flip = flip;
    setFlip(flip);
    hitbox.setRectangle({1, 5}, {15, 11});
    collisionType = TA_COLLISION_SOLID;
    updatePosition();
}

bool TA_UnderwaterGun::update() {
    if(timer < cooldown) {
        timer += TA::elapsedTime;
        return true;
    }

    TA_Point characterPos = objectSet->getCharacterPosition();
    TA_Point delta = {characterPos.x - position.x - 8, characterPos.y - position.y - 8};

    if(std::abs(delta.y) <= 24 &&
        ((!flip && 0 <= delta.x && delta.x <= 256) || (flip && -256 <= delta.x && delta.x <= 0))) {
        objectSet->spawnObject<TA_UnderwaterBullet>(
            TA_Point(flip ? position.x - 7.01 : position.x + 15.01, position.y + 4), flip);
        timer = 0;
    }

    return true;
}

void TA_UnderwaterBullet::load(TA_Point position, bool flip) {
    loadFromToml("objects/underwater_bullet.toml");
    this->position = position;
    this->flip = flip;
    hitbox.setRectangle({0, 0}, {8, 7});
    collisionType = TA_COLLISION_DAMAGE;
    updatePosition();
    smallExplosionSound.load("sound/explosion_small.ogg", TA_SOUND_CHANNEL_SFX3);
}

bool TA_UnderwaterBullet::update() {
    position.x += (flip ? -1.0F : 1.0F) * speed;
    updatePosition();
    int flags = objectSet->checkCollision(hitbox);

    if((flags & (TA_COLLISION_SOLID | TA_COLLISION_CHARACTER)) != 0) {
        objectSet->spawnObject<TA_Explosion>(position - TA_Point(4, 4), 0, TA_EXPLOSION_ENEMY);
        if((flags & TA_COLLISION_CHARACTER) != 0) {
            smallExplosionSound.play();
        }
        return false;
    }

    return true;
}
