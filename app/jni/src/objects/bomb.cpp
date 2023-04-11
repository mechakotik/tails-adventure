#include "bomb.h"
#include "tools.h"
#include "objects/explosion.h"

void TA_Bomb::load(TA_Point newPosition, bool newDirection, TA_BombMode newMode) {
    TA_Sprite::load("objects/bomb.png");
    explosionSound.load("sound/explosion.ogg", TA_SOUND_CHANNEL_SFX);
    position = newPosition;
    setPosition(position);
    hitbox.setRectangle(TA_Point(1.5, 2.5), TA_Point(11.5, 13.5));
    mode = newMode;
    if(mode == TA_BOMB_MODE_AIR) {
        velocity = startVelocity;
        timer = 10;
    }
    if (mode == TA_BOMB_MODE_CROUCH) {
        velocity = startCrouchVelocity;
    }
    else if (mode == TA_BOMB_MODE_HELITAIL) {
        velocity = startHelitailVelocity;
        timer =  10;
    }
    else {
        velocity = startVelocity;
    }
    direction = newDirection;
    if(direction) {
        position.x;
        velocity.x *= -1;
    }
}

bool TA_Bomb::checkPawnCollision(TA_Polygon &hitbox)
{
    int flags = 0;
    objectSet->checkCollision(hitbox, flags);
    return (flags & TA_COLLISION_SOLID) || (flags & TA_COLLISION_HALF_SOLID) || (flags & TA_COLLISION_PUSHABLE);
}

bool TA_Bomb::update()
{
    int moveTime = (mode == TA_BOMB_MODE_DEFAULT ? 6 : 8);
    bool flag1 = (timer <= moveTime);
    timer += TA::elapsedTime;
    bool flag2 = (timer >= moveTime);

    if(flag1 && flag2) {
        if(mode == TA_BOMB_MODE_DEFAULT) {
            position = position + TA_Point(9 * (direction ? -1 : 1), -6);
        }
        else {
            position = position + TA_Point(5 * (direction ? -1 : 1), -6);
        }
    }
    else if(timer >= moveTime) {
        velocity.y += grv * TA::elapsedTime;
        moveAndCollide(TA_Point(2, 3), TA_Point(11, 13), velocity);
        int flags;
        hitbox.setPosition(position);
        objectSet->checkCollision(hitbox, flags);
        if((flags & TA_COLLISION_SOLID) || (flags & TA_COLLISION_HALF_SOLID) || (flags & TA_COLLISION_DAMAGE) || (flags & TA_COLLISION_PUSHABLE)) {
            objectSet->spawnObject<TA_Explosion>(position);
            for(int i = 1; i <= 3; i ++) {
                TA_Point explosionPosition = position + TA_Point(int(TA::random::next() % 7) - 3, int(TA::random::next() % 7) - 3);
                explosionSound.play();
                objectSet->spawnObject<TA_Explosion>(explosionPosition, i * 16);
            }
            return false;
        }
    }

    setPosition(position);
    return true;
}
