#include "enemy_mine.h"
#include <cmath>
#include "explosion.h"

void TA_EnemyMine::load(TA_Point position, bool fall) {
    TA_Sprite::load("objects/enemy_mine.png");
    hitbox.setRectangle(TA_Point(0, 0), TA_Point(14, 14));
    collisionType = TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;
    timer = TA::random::next() % int(interval);
    this->startPosition = this->position = position;
    this->fall = fall;
    smallExplosionSound.load("sound/explosion_small.ogg", TA_SOUND_CHANNEL_SFX3);
}

bool TA_EnemyMine::update() {
    auto func = [](float x) {
        float y;
        if(int(x) % 2 == 0) {
            y = std::fmod(x, 1) - 0.5;
        } else {
            y = 0.5 - std::fmod(x, 1);
        }
        return float(1) / (1 + exp(-6 * y));
    };

    timer += TA::elapsedTime;
    TA_Point delta{(1 - func(timer / interval)) * 4, func(timer / (interval * 2)) * 8};

    if(fall) {
        ysp = std::min(maxYSpeed, ysp + gravity * TA::elapsedTime);
        position.y += ysp * TA::elapsedTime;
    } else {
        position = startPosition + delta;
    }
    updatePosition();

    int flags = objectSet->checkCollision(hitbox);
    if((flags & (TA_COLLISION_SOLID | TA_COLLISION_ATTACK | TA_COLLISION_CHARACTER)) != 0) {
        objectSet->spawnObject<TA_Explosion>(position - TA_Point(1, 1), 0, TA_EXPLOSION_ENEMY);
        if((flags & TA_COLLISION_CHARACTER) != 0) {
            smallExplosionSound.play();
        }
        return false;
    }

    return true;
}
