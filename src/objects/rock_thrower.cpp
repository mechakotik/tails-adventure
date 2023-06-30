#include "rock_thrower.h"
#include "dead_kukku.h"

void TA_RockThrower::load(TA_Point position, bool direction)
{
    this->position = position;
    this->direction = direction;

    TA_Sprite::load("objects/rock_thrower.png", 16, 29);
    TA_Sprite::loadAnimationsFromFile("objects/rock_thrower.xml");
    TA_Sprite::setFlip(direction);
    TA_Sprite::setAnimation("idle");

    hitbox.setRectangle(TA_Point(2, 1), TA_Point(14, 29));
    updatePosition();
}

bool TA_RockThrower::update()
{
    if(idle) {
        updateIdle();
    }
    else {
        if(shouldThrowRock()) {
            throwRock();
        }
        prevFrame = getAnimationFrame();
    }

    if(shouldBeDestroyed()) {
        destroy();
        return false;
    }
    return true;
}

void TA_RockThrower::updateIdle()
{
    TA_Point distance = getDistanceToCharacter();
    if(abs(distance.x) <= 128 && abs(distance.y) <= 64) {
        timer += TA::elapsedTime;
    }
    else {
        timer = 0;
    }
    if(timer > idleTime) {
        idle = false;
        setAnimation("throw");
    }
}

bool TA_RockThrower::shouldThrowRock()
{
    int frame = getAnimationFrame();
    if(frame == prevFrame) {
        return false;
    }
    if(!isCloseToCharacter()) {
        return false;
    }
    return frame == 0 || frame == 4 || frame == 10;
}

bool TA_RockThrower::isCloseToCharacter()
{
    TA_Point distance = getDistanceToCharacter();
    return abs(distance.x) <= 160 && abs(distance.y) <= 120;
}

void TA_RockThrower::throwRock()
{
    objectSet->spawnObject<TA_EnemyRock>(getRockPosition(), getRockVelocity());
}

TA_Point TA_RockThrower::getRockPosition()
{
    if(direction) {
        return position + TA_Point(9, 13);
    }
    return position + TA_Point(1, 13);
}

TA_Point TA_RockThrower::getRockVelocity()
{
    TA_Point distance = getDistanceToCharacter();
    TA_Point velocity;
    velocity.x = speed * (direction ? 1 : -1);

    if(abs(distance.y) <= 32) {
        velocity.y = -2.25;
    }
    else if(distance.y < 0) {
        velocity.y = -3.5;
    }
    else {
        velocity.y = -0.75;
    }

    return velocity;
}

bool TA_RockThrower::shouldBeDestroyed()
{
    if(objectSet->checkCollision(hitbox) & (TA_COLLISION_BOMB | TA_COLLISION_EXPLOSION)) {
        return true;
    }
    return false;
}

void TA_RockThrower::destroy()
{
    objectSet->spawnObject<TA_DeadKukku>(position - TA_Point(4, 1));
}

void TA_EnemyRock::load(TA_Point position, TA_Point velocity)
{
    this->position = position;
    this->velocity = velocity;

    TA_Sprite::load("objects/enemy_rock.png", 6, 7);
    TA_Sprite::loadAnimationsFromFile("objects/enemy_rock.xml");
    TA_Sprite::setAnimation("rock");

    hitbox.setRectangle(TA_Point(0, 0), TA_Point(6, 7));
    updatePosition();
}

bool TA_EnemyRock::update()
{
    updateVelocity();
    updateCollision();
    updatePosition();

    if(TA::equal(velocity.x, 0) && TA::equal(velocity.y, 0)) {
        return false;
    }
    return true;
}

void TA_EnemyRock::updateVelocity()
{
    double add = (ground ? friction : airDrag) * TA::elapsedTime;
    if(velocity.x > add) {
        velocity.x -= add;
    }
    else if(velocity.x < -add) {
        velocity.x += add;
    }
    else {
        velocity.x = 0;
    }

    if(ground) {
        velocity.y = 0;
    }
    else {
        velocity.y += gravity * TA::elapsedTime;
        velocity.y = std::min(velocity.y, maxFallSpeed);
    }
}

void TA_EnemyRock::updateCollision()
{
    int flags = moveAndCollide(TA_Point(0, 0), TA_Point(6, 7), velocity * TA::elapsedTime, ground);
    
    ground = ((flags & TA_GROUND_COLLISION) != 0);
    if(flags & TA_WALL_COLLISION) {
        velocity.x = 0;
    }
    if(flags & TA_CEIL_COLLISION) {
        velocity.y = 0;
    }
}

bool TA_EnemyRock::checkPawnCollision(TA_Polygon &hitbox)
{
    if(objectSet->checkCollision(hitbox) & (TA_COLLISION_SOLID | TA_COLLISION_HALF_SOLID)) {
        return true;
    }
    return false;
}
