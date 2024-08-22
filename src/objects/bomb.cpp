#include "bomb.h"
#include "tools.h"
#include "objects/explosion.h"
#include "character.h"
#include "error.h"
#include "napalm_fire.h"

void TA_Bomb::load(TA_Point newPosition, bool newDirection, TA_BombMode newMode) {
    TA_Sprite::load("objects/bomb.png", 16, 16);
    TA_Sprite::loadAnimationsFromFile("objects/bomb.xml");
    explosionSound.load("sound/explosion.ogg", TA_SOUND_CHANNEL_SFX3);

    position = newPosition;
    setPosition(position);
    hitbox.setRectangle(topLeft - TA_Point(0.5, 0.5), bottomRight + TA_Point(0.5, 0.5));

    mode = newMode;
    if(mode == TA_BOMB_MODE_AIR) {
        TA_Point addVelocity = objectSet->getLinks().character->getVelocity();
        addVelocity.x = abs(addVelocity.x);
        velocity = startVelocity + addVelocity;
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
    if(destroyed) {
        timer += TA::elapsedTime;
        if(timer > destroyedTime) {
            return false;
        }
        return true;
    }

    int moveTime = (mode == TA_BOMB_MODE_DEFAULT ? 6 : 8);
    bool flag1 = (timer <= moveTime);
    timer += TA::elapsedTime;
    bool flag2 = (timer >= moveTime);

    if(flag1 && flag2) {
        if(mode == TA_BOMB_MODE_DEFAULT) {
            position = position + TA_Point(9 * (direction ? -1 : 1), -6);
        }
        else {
            position = position + TA_Point(5 * (direction ? -1 : 1), crouchThrowHeight);
        }
        updatePosition();
        if(checkPawnCollision(hitbox)) {
            explode();
            return true;
        }
    }

    if(timer >= moveTime) {
        velocity.y += grv * speed * speed * TA::elapsedTime;

        TA_Point velocityAdd = TA_Point(0, 0);
        if(ground) {
            int flags = objectSet->checkCollision(hitbox);
            if(flags & TA_COLLISION_CONVEYOR_BELT_LEFT) {
                velocityAdd = TA_Point(-1, 0);
            }
            if(flags & TA_COLLISION_CONVEYOR_BELT_RIGHT) {
                velocityAdd = TA_Point(1, 0);
            }
        }

        int moveFlags = moveAndCollide(topLeft, bottomRight, (velocity + velocityAdd) * TA::elapsedTime, ground);

        if(moveFlags & TA_GROUND_COLLISION) {
            velocity.y = std::min(double(0), velocity.y);
            ground = true;
        }
        else {
            ground = false;
        }

        if(moveFlags & TA_WALL_COLLISION) {
            velocity.x = 0;
        }
        if(moveFlags & TA_CEIL_COLLISION) {
            velocity.y = std::max(double(0), velocity.y);
        }
        if(shouldExplode()) {
            explode();
            destroyed = true;
            timer = 0;
        }
    }

    setPosition(position);
    return true;
}

bool TA_Bomb::shouldExplode()
{
    hitbox.setPosition(position);
    int flags = objectSet->checkCollision(hitbox);

    if(flags & (TA_COLLISION_SOLID | TA_COLLISION_HALF_SOLID | TA_COLLISION_DAMAGE | TA_COLLISION_PUSHABLE)) {
        return true;
    }
    return false;
}

void TA_Bomb::explode()
{
    objectSet->spawnObject<TA_Explosion>(position);
    hitbox.setRectangle(topLeft - TA_Point(2, 2), bottomRight + TA_Point(2, 2));
    for(int i = 1; i <= 3; i ++) {
        TA_Point explosionPosition = position + TA_Point(int(TA::random::next() % 7) - 3, int(TA::random::next() % 7) - 3);
        explosionSound.play();
        objectSet->spawnObject<TA_Explosion>(explosionPosition, i * 16);
    }
}

void TA_Bomb::draw()
{
    if(!destroyed) {
        TA_Sprite::draw();
    }
}

void TA_RemoteBomb::load(TA_Point newPosition, bool newDirection, TA_BombMode mode)
{
    topLeft = TA_Point(1, 4);
    bottomRight = TA_Point(14, 16);
    speed = 1;
    startVelocity = {1.35 * speed, -1 * speed};
    startCrouchVelocity = {1 * speed, -0.7 * speed};
    crouchThrowHeight = 0;
    
    TA_Bomb::load(newPosition, newDirection, mode);
    setAnimation("remote");
}

bool TA_RemoteBomb::shouldExplode()
{
    if(objectSet->getLinks().character->getBombDestroySignal()) {
        return true;
    }
    
    hitbox.setPosition(position);
    if(objectSet->checkCollision(hitbox) & TA_COLLISION_DAMAGE) {
        return true;
    }
    return false;
}

bool TA_RemoteBomb::update()
{
    if(TA::equal(velocity.y, 0)) {
        if(velocity.x > 0) {
            velocity.x = std::max(double(0), velocity.x - friction * speed * speed * TA::elapsedTime);
        }
        else {
            velocity.x = std::min(double(0), velocity.x + friction * speed * speed * TA::elapsedTime);
        }
    }

    return TA_Bomb::update();
}

void TA_NapalmBomb::load(TA_Point newPosition, bool newDirection, TA_BombMode mode)
{
    topLeft = {4, 4};
    bottomRight = {12, 12};
    startVelocity = {1.35 * speed, -1 * speed};
    startCrouchVelocity = {1 * speed, -0.7 * speed};
    crouchThrowHeight = 0;

    TA_Bomb::load(newPosition, newDirection, mode);
    setAnimation("napalm");
    hitbox.setRectangle(topLeft + TA_Point(0.5, 0), bottomRight + TA_Point(-0.5, 0.5));
}

bool TA_NapalmBomb::shouldExplode()
{
    hitbox.setPosition(position);
    int flags = objectSet->checkCollision(hitbox);

    if(flags & (TA_COLLISION_SOLID | TA_COLLISION_HALF_SOLID | TA_COLLISION_PUSHABLE)) {
        return true;
    }
    return false;
}

void TA_NapalmBomb::explode()
{
    objectSet->spawnObject<TA_NapalmFire>(position + TA_Point(4, -15), velocity.x);
}

bool TA_NapalmBomb::update()
{
    return TA_Bomb::update();
}
