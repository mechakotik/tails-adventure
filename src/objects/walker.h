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
    const float speed = 0.25;
    const float fireTime = 80;
    const float standTime = 20;

    void updateIdle();
    void updateMove();
    void updateFire();
    void updateMoveAway();
    TA_Point getDistanceToCharacter();

    int rangeLeft, rangeRight;
    bool direction;
    TA_Point velocity;
    float timer = 0;
    bool alwaysIdle = false;
    TA_WalkerState state = TA_WALKER_STATE_IDLE;

public:
    using TA_Object::TA_Object;
    void load(TA_Point newPosition, int range, bool flip);
    bool update() override;
};

class TA_WalkerBullet : public TA_Object { // TODO: reimplement TA_WalkerBullet as inheritor of TA_Bullet
private:
    bool direction;

    const float speed = 1;

public:
    using TA_Object::TA_Object;
    void load(TA_Point newPosition, bool newDirection);
    bool update() override;
};

#endif // TA_WALKER_H
