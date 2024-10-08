#ifndef TA_NEZU_H
#define TA_NEZU_H

#include "object_set.h"

class TA_Nezu : public TA_Object {
private:
    const double speed = 0.5;
    const double placeTime = 30;
    const double attackTime = 60;
    const double gravity = 0.125;
    const double maxFallSpeed = 3;

    TA_Point getDistanceToCharacter();
    bool isBadPosition(TA_Point position);
    bool isCloseToCharacter();
    bool isGoingToFall();

    void updateIdle();
    void updateWalk();
    void updateAttack();
    bool updateFall();
    void placeBomb();
    void destroy() override;

    enum State {
        STATE_IDLE,
        STATE_WALK,
        STATE_ATTACK,
        STATE_FALL
    };

    State state = STATE_IDLE;
    bool direction = 0, bombPlaced = 0;
    double timer = 0, fallSpeed = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    int getCollisionType() override {return TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;}
};

class TA_NezuBomb : public TA_Object {
private:
    const double waitTime = 30;

    double timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update();
};

#endif // TA_NEZU_H
