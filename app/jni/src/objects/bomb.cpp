#include "bomb.h"
#include "engine/tools.h"

void TA_Bomb::load(TA_Point newPosition, bool newDirection)
{
    TA_Sprite::load("tools/bomb.png");
    position = newPosition;
    setPosition(position);
    velocity = startVelocity;
    direction = newDirection;
    if(direction) {
        position.x;
        velocity.x *= -1;
    }
}

bool TA_Bomb::checkPawnCollision(TA_Polygon hitbox)
{
    int flags = 0;
    objectSet->checkCollision(hitbox, flags);
    return flags != 0;
}

bool TA_Bomb::update()
{
    bool flag1 = (timer <= 6);
    timer += TA::elapsedTime;
    bool flag2 = (timer >= 6);

    if(flag1 && flag2) {
        position = position + TA_Point(9 * (direction ? -1 : 1), -6);
    }
    else if(timer >= 6) {
        velocity.y += grv * TA::elapsedTime;
        int flags = moveAndCollide(TA_Point(2, 4), TA_Point(11, 13), velocity);
        if(flags != 0) {
            objectSet->spawnExplosion(position);
            for(int i = 1; i <= 3; i ++) {
                TA_Point explosionPosition = position + TA_Point(int(TA::random::next() % 7) - 3, int(TA::random::next() % 7) - 3);
                objectSet->spawnExplosion(explosionPosition, i * 16);
            }
            return false;
        }
    }

    setPosition(position);
    return true;
}
