#ifndef TA_ROCK_THROWER_H
#define TA_ROCK_THROWER_H

#include "object_set.h"

class TA_RockThrower : public TA_Object {
private:
    const float speed = 1.25;
    const float idleTime = 120;

    void updateIdle();
    bool isCloseToCharacter();
    bool shouldThrowRock();
    void throwRock();
    bool shouldBeDestroyed();
    void destroy() override;

    TA_Point getRockPosition();
    TA_Point getRockVelocity();

    int prevFrame = -1;
    bool direction, idle = true;
    float timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, bool direction);
    bool update() override;
};

class TA_EnemyRock : public TA_Object {
private:
    const float gravity = 0.125;
    const float friction = 0.1;
    const float airDrag = 0.0075;
    const float maxFallSpeed = 2.5;

    void updateVelocity();
    void updateCollision();

    TA_Point velocity;
    bool ground = false;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, TA_Point velocity);
    bool update() override;
};

#endif // TA_ROCK_THROWER_H
