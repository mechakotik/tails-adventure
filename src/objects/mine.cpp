#include "mine.h"
#include "explosion.h"
#include "geometry.h"

void TA_Mine::load(TA_Point position, float xsp) {
    loadFromToml("objects/mine.toml");
    this->position = position;
    velocity = {xsp, 0};

    hitbox.setRectangle({1, 1}, {7, 7});
    collisionType = TA_COLLISION_ATTACK | TA_COLLISION_BOMB | TA_COLLISION_DRILL;
    updatePosition();

    explosionSound.load("sound/explosion.ogg", TA_SOUND_CHANNEL_SFX3);
}

bool TA_Mine::update() {
    if(!destroyed) {
        velocity.y = std::min(maxYSpeed, velocity.y + gravity * TA::elapsedTime);
        if(velocity.x > 0) {
            velocity.x = std::max(0.0F, velocity.x - drag * TA::elapsedTime);
        } else {
            velocity.x = std::min(0.0F, velocity.x + drag * TA::elapsedTime);
        }

        position += velocity * TA::elapsedTime;
        updatePosition();

        if((objectSet->checkCollision(hitbox) & (TA_COLLISION_SOLID | TA_COLLISION_TARGET)) != 0) {
            destroyed = true;
            hitbox.setRectangle({-2, -2}, {18, 18});
            collisionType ^= TA_COLLISION_BOMB;
            objectSet->spawnObject<TA_Explosion>(position - TA_Point(4, 4), 0, TA_EXPLOSION_CHARACTER);
            explosionSound.play();
            for(int i = 1; i <= 3; i++) {
                TA_Point explosionPosition =
                    position + TA_Point(int(TA::random::next() % 7) - 3, int(TA::random::next() % 7) - 3);
                objectSet->spawnObject<TA_Explosion>(explosionPosition, i * 16, TA_EXPLOSION_NEUTRAL);
            }
        }
    } else {
        timer += TA::elapsedTime;
        if(timer > 10) {
            return false;
        }
    }

    return true;
}
