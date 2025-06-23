#ifndef TA_BOMB_THROWER_H
#define TA_BOMB_THROWER_H

#include "object_set.h"

class TA_BombThrower : public TA_Object {
private:
    const float idleTime = 30;
    const float walkBackTime = 100;
    const float speed = 0.7;

    void updateIdle();
    void updateWalkForward();
    void updateWalkBack();
    void initAttack();
    void updateAttack();
    TA_Point getDistanceToCharacter();
    bool shouldBeDestroyed();
    void destroy() override;

    enum State { STATE_IDLE, STATE_WALK_FORWARD, STATE_WALK_BACK, STATE_ATTACK };

    State state = STATE_IDLE;
    float leftX, rightX;
    bool direction = false;
    float timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, float leftX, float rightX);
    bool update() override;
};

class TA_EnemyBomb : public TA_Object {
private:
    const float grv = 0.125;
    const TA_Point topLeft{1, 6}, bottomRight{7, 12};

    TA_Point velocity{-1.2, -2.4};

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    bool checkPawnCollision(TA_Rect& hitbox) override;
};

#endif // TA_BOMB_THROWER_H
