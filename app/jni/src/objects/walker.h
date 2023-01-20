#ifndef TA_WALKER_H
#define TA_WALKER_H

#include "engine/object_set.h"

enum TA_WalkerState
{
    TA_WALKER_STATE_IDLE,
    TA_WALKER_STATE_MOVE,
    TA_WALKER_STATE_FIRE,
    TA_WALKER_STATE_MOVE_AWAY,
};

class TA_Walker : public TA_Object {
private:
    int rangeLeft, rangeRight;
    bool direction, dead = false;
    TA_Point velocity;
    double timer = 0;
    bool alwaysIdle = false;
    TA_WalkerState state = TA_WALKER_STATE_IDLE;

    const double speed = 0.25;
    const double grv = 0.125;
    const double startYsp = -2.5;
    const double fireTime = 90, deathTime = 500;

public:
    using TA_Object::TA_Object;
    void load(TA_Point newPosition, int range, bool flip);
    bool update();
    TA_CollisionType getCollisionType() override;
};

class TA_WalkerBullet : public TA_Object {
private:
    bool direction;

    const double speed = 1;

public:
    using TA_Object::TA_Object;
    void load(TA_Point newPosition, bool newDirection);
    bool update();
    TA_CollisionType getCollisionType() override {return TA_COLLISION_DAMAGE;}
};

#endif // TA_WALKER_H
