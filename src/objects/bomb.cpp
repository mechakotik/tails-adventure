#include "bomb.h"
#include "tools.h"
#include "objects/explosion.h"
#include "character.h"

void TA_Bomb::load(TA_Point newPosition, bool newDirection, TA_BombMode newMode) {
    TA_Sprite::load("objects/bomb.png");
    explosionSound.load("sound/explosion.ogg", TA_SOUND_CHANNEL_SFX2);
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
    if(timer >= moveTime) {
        velocity.y += grv * speed * speed * TA::elapsedTime;
        int moveFlags = moveAndCollide(TA_Point(2, 3), TA_Point(11, 13), velocity);
        if(moveFlags & TA_GROUND_COLLISION) {
            velocity.y = std::min(double(0), velocity.y);
        }
        if(moveFlags & TA_WALL_COLLISION) {
            velocity.x = 0;
        }
        if(moveFlags & TA_CEIL_COLLISION) {
            velocity.y = std::max(double(0), velocity.y);
        }

        hitbox.setPosition(position);
        int flags = objectSet->checkCollision(hitbox);
        if((flags & destroyFlags) || shouldExplode()) {
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

void TA_RemoteBomb::load(TA_Point newPosition, bool newDirection, TA_BombMode mode)
{
    speed = 1;
    destroyFlags = TA_COLLISION_DAMAGE;
    startVelocity = {1.35 * speed, -1 * speed};
    startCrouchVelocity = {1 * speed, -0.7 * speed};
    TA_Bomb::load(newPosition, newDirection, mode);
}

bool TA_RemoteBomb::shouldExplode()
{
    if(objectSet->getLinks().character->getBombDestroySignal()) {
        return true;
    }
    if(timer > waitTime) {
        return true;
    }
    return false;
}

bool TA_RemoteBomb::update()
{
    if(TA::equal(velocity.y, 0)) {
        if(velocity.x > 0) {
            velocity.x = std::max(double(0), velocity.x - friction * speed * speed);
        }
        else {
            velocity.x = std::min(double(0), velocity.x + friction * speed * speed);
        }
    }

    if(TA::equal(velocity.x, 0) && TA::equal(velocity.y, 0)) {
        timer += TA::elapsedTime;
    }
    else {
        timer = 0;
    }

    return TA_Bomb::update();
}
