#include "enemy_mine.h"
#include "explosion.h"
#include <cmath>

void TA_EnemyMine::load(TA_Point position)
{
    TA_Sprite::load("objects/mine.png");
    hitbox.setRectangle(TA_Point(0, 0), TA_Point(14, 14));
    timer = TA::random::next() % int(interval);
    this->startPosition = position;
}

bool TA_EnemyMine::update()
{
    auto func = [] (double x) {
        double y;
        if(int(x) % 2 == 0) {
            y = std::fmod(x, 1) - 0.5;
        }
        else {
            y = 0.5 - std::fmod(x, 1);
        }
        return double(1) / (1 + exp(-6 * y));
    };

    timer += TA::elapsedTime;
    TA_Point delta{(1 - func(timer / interval)) * 4, func(timer / (interval * 2)) * 8};
    position = startPosition + delta;
    updatePosition();

    if(objectSet->checkCollision(hitbox) & (TA_COLLISION_EXPLOSION | TA_COLLISION_CHARACTER)) {
        objectSet->spawnObject<TA_Explosion>(position - TA_Point(1, 1), 0, TA_EXPLOSION_ENEMY);
        return false;
    }
    
    return true;
}
