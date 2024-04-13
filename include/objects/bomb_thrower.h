#ifndef TA_BOMB_THROWER_H
#define TA_BOMB_THROWER_H

#include "object_set.h"

class TA_BombThrower : public TA_Object {
private:
    const double idleTime = 30;
    const double walkBackTime = 100;
    const double speed = 0.7;

    void updateIdle();
    void updateWalkForward();
    void updateWalkBack();
    void initAttack();
    void updateAttack();
    TA_Point getDistanceToCharacter();
    bool shouldBeDestroyed();
    void destroy() override;

    enum State {
        STATE_IDLE,
        STATE_WALK_FORWARD,
        STATE_WALK_BACK,
        STATE_ATTACK
    };

    State state = STATE_IDLE;
    double leftX, rightX;
    bool direction = false;
    double timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, double leftX, double rightX);
    bool update() override;
    TA_CollisionType getCollisionType() override {return TA_COLLISION_DAMAGE;}
};

class TA_EnemyBomb : public TA_Object {
private:
    const double grv = 0.125;
    const TA_Point topLeft{1, 6}, bottomRight{7, 12};

    TA_Point velocity{-1.2, -2.4};

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    TA_CollisionType getCollisionType() override {return TA_COLLISION_DAMAGE;}
    bool checkPawnCollision(TA_Polygon &hitbox) override;
};

#endif // TA_BOMB_THROWER_H
