#ifndef TA_SPEEDY_H
#define TA_SPEEDY_H

#include "object_set.h"

class TA_Speedy : public TA_Object {
private:
    const TA_Point startShowVelocity{-3, 4};
    const double gravity = 0.125;
    const double showWaitTime = 30;
    const double attackSpeed = 1.5;
    const double flyUpSpeed = 5;
    const double flyUpPhase1Speed = 2;
    const double flyUpPhase2Speed = 7;

    enum State {
        STATE_SHOW,
        STATE_IDLE,
        STATE_ATTACK,
        STATE_FLY_UP_SIMPLE,
        STATE_FLY_UP
    } state = STATE_SHOW;

    void loadSprite();
    void initShow();
    void initIdle();
    void initAttack();
    void initFlyUp();

    void updateShow();
    void updateIdle();
    void updateAttack();
    void updateFlyUpSimple();
    void updateFlyUp();

    void updateFlip();

    TA_Point velocity;
    double timer = 0;
    bool waiting = false;

    int flyUpPhase;
    double flyUpPhase2Y;

public:
    using TA_Object::TA_Object;
    void load();
    bool update();
    TA_CollisionType getCollisionType() override;
};

#endif // TA_SPEEDY_H
