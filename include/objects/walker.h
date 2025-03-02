#ifndef TA_WALKER_H
#define TA_WALKER_H

#include "object_set.h"

enum TA_WalkerState {
    TA_WALKER_STATE_IDLE,
    TA_WALKER_STATE_MOVE,
    TA_WALKER_STATE_FIRE,
    TA_WALKER_STATE_MOVE_AWAY,
};

class TA_Walker : public TA_Object {
private:
    const double speed = 0.25;
    const double fireTime = 80;
    const double standTime = 20;

    void updateIdle();
    void updateMove();
    void updateFire();
    void updateMoveAway();
    TA_Point getDistanceToCharacter();

    int rangeLeft, rangeRight;
    bool direction;
    TA_Point velocity;
    double timer = 0;
    bool alwaysIdle = false;
    TA_WalkerState state = TA_WALKER_STATE_IDLE;

public:
    using TA_Object::TA_Object;
    void load(TA_Point newPosition, int range, bool flip);
    bool update() override;
    int getCollisionType() override { return TA_COLLISION_DAMAGE | TA_COLLISION_TARGET; }
};

class TA_WalkerBullet : public TA_Object { // TODO: reimplement TA_WalkerBullet as inheritor of TA_Bullet
private:
    bool direction;

    const double speed = 1;

public:
    using TA_Object::TA_Object;
    void load(TA_Point newPosition, bool newDirection);
    bool update() override;
    int getCollisionType() override { return TA_COLLISION_DAMAGE; }
};

#endif // TA_WALKER_H
