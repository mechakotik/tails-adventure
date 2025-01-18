#include "jumper.h"
#include "dead_kukku.h"
#include "tools.h"

void TA_Jumper::load(TA_Point position)
{
    this->position = position;

    TA_Sprite::load("objects/jumper.png", 16, 31);
    TA_Sprite::loadAnimationsFromFile("objects/jumper.xml");

    hitbox.setRectangle(TA_Point(1, 1), TA_Point(15, 31));
    updatePosition();
}

bool TA_Jumper::update()
{
    switch(state) {
        case STATE_IDLE:
            updateIdle();
            break;
        case STATE_AIM:
            updateAim();
            break;
        case STATE_JUMP:
            updateJump();
            break;
    }

    updatePosition();

    if(shouldBeDestroyed()) {
        destroy();
        return false;
    }
    return true;
}

void TA_Jumper::updateIdle()
{
    setAnimation("jump");
    updateDirection();
    
    if(isCloseToCharacter()) {
        timer += TA::elapsedTime;
        if(timer > idleTime) {
            initAim();
        }
    }
    else {
        timer = 0;
    }
}

void TA_Jumper::initAim()
{
    state = STATE_AIM;
    setAnimation("aim");
}

void TA_Jumper::updateAim()
{
    if(!isAnimated()) {
        setJumpVelocity();
        state = STATE_JUMP;
    }
}

void TA_Jumper::setJumpVelocity()
{
    int tileDistance = (std::abs(getDistanceToCharacter().x) + 8) / 16;
    if(tileDistance > 3 && TA::random::next() % 3 == 0) {
        tileDistance = 3;
    }

    if(tileDistance > 3 || tileDistance == 0) {
        velocity.x = 0;
        velocity.y = -2.3;
        return;
    }

    velocity.x = tileDistance * jumpSpeed * (direction ? 1 : -1);
    double jumpTime = tileDistance * 16 / std::abs(velocity.x), deltaY = getDistanceToCharacter().y;
    velocity.y = (deltaY - jumpTime * (jumpTime - 1) * gravity / 2) / jumpTime;
}

void TA_Jumper::updateJump()
{
    setAnimation("jump");

    velocity.y += gravity * TA::elapsedTime;
    int flags = moveAndCollide(TA_Point(4, 1), TA_Point(12, 31), velocity * TA::elapsedTime);

    if(flags & TA_CEIL_COLLISION) {
        velocity.y = std::max(velocity.y, double(0));
    }
    if(flags & TA_WALL_COLLISION) {
        velocity.x = 0;
    }
    if(flags & TA_GROUND_COLLISION) {
        timer = 0;
        state = STATE_IDLE;
    }
}

void TA_Jumper::updateDirection()
{
    TA_Point distance = getDistanceToCharacter();
    direction = (distance.x > 0);
    setFlip(direction);
}

bool TA_Jumper::isCloseToCharacter()
{
    TA_Point distance = getDistanceToCharacter();
    return abs(distance.x) <= 128 && abs(distance.y) <= 96;
}

bool TA_Jumper::checkPawnCollision(TA_Polygon &hitbox)
{
    int flags = objectSet->checkCollision(hitbox);
    if(flags & (TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP)) {
        return true;
    }
    return false;
}

bool TA_Jumper::shouldBeDestroyed()
{
    if(objectSet->checkCollision(hitbox) & TA_COLLISION_ATTACK) {
        return true;
    }
    return false;
}

void TA_Jumper::destroy()
{
    objectSet->spawnObject<TA_DeadKukku>(position - TA_Point(4, 1));
    objectSet->resetInstaShield();
}
