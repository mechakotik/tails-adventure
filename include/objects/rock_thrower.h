#ifndef TA_ROCK_THROWER_H
#define TA_ROCK_THROWER_H

#include "object_set.h"

class TA_RockThrower : public TA_Object {
private:
    const double speed = 1.25;
    const double idleTime = 120;

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
    double timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, bool direction);
    bool update() override;
    TA_CollisionType getCollisionType() override {return TA_COLLISION_DAMAGE;}
};

class TA_EnemyRock : public TA_Object {
private:
    const double gravity = 0.125;
    const double friction = 0.1;
    const double airDrag = 0.0075;
    const double maxFallSpeed = 2.5;

    void updateVelocity();
    void updateCollision();

    TA_Point velocity;
    bool ground = false;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, TA_Point velocity);
    bool update() override;
    TA_CollisionType getCollisionType() override {return TA_COLLISION_DAMAGE;}
    bool checkPawnCollision(TA_Polygon &hitbox) override;
};

#endif // TA_ROCK_THROWER_H
