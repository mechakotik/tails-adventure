#ifndef TA_SPEEDY_H
#define TA_SPEEDY_H

#include "object_set.h"

class TA_Speedy : public TA_Object {
private:
    const TA_Point startShowVelocity{-3, 4};
    const float gravity = 0.125;
    const float showWaitTime = 30;
    const float attackSpeed = 1.5;
    const float flyUpSpeed = 5;
    const float flyUpPhase1Speed = 2;
    const float flyUpPhase2Speed = 7;
    const float flyAwayAcceleration = 0.125;

    enum State {
        STATE_SHOW,
        STATE_IDLE,
        STATE_ATTACK,
        STATE_FLY_UP_SIMPLE,
        STATE_FLY_UP,
        STATE_END_SEQUENCE,
        STATE_WAIT_ITEM,
        STATE_FLY_AWAY,
        STATE_TRANSITION
    } state = STATE_SHOW;

    bool isComplete();
    void loadSprite();
    void initShow();
    void initIdle();
    void initAttack();
    void initFlyUp();
    void initEndSequence();

    void updateShow();
    void updateIdle();
    void updateAttack();
    void updateFlyUpSimple();
    void updateFlyUp();
    void updateWaitItem();
    void updateFlyAway();

    void updateEndSequence();
    void updateEndSequencePhase0();
    void updateEndSequencePhase1();
    void updateEndSequencePhase2();
    void updateEndSequencePhase3();
    void updateEndSequencePhase4();
    void updateEndSequencePhase5();

    void updateFlip();
    void doTransition();

    TA_Point velocity;
    float timer = 0;
    bool waiting = false;

    int flyUpPhase, endSequencePhase;
    float flyUpPhase2Y;

    TA_Sprite characterPlaceholder;
    TA_Point cpPosition, cpVelocity;

public:
    using TA_Object::TA_Object;
    void load();
    bool update() override;
    void draw() override;
    int getCollisionType() override;
};

#endif // TA_SPEEDY_H
