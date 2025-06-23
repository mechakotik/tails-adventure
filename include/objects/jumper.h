#ifndef TA_JUMPER_H
#define TA_JUMPER_H

#include "object_set.h"

class TA_Jumper : public TA_Object {
private:
    const float jumpSpeed = 0.3;
    const float gravity = 0.085;
    const float idleTime = 40;

    enum State { STATE_IDLE, STATE_AIM, STATE_JUMP };

    void updateIdle();
    void updateAim();
    void updateJump();
    void initAim();

    void setJumpVelocity();
    void updateDirection();
    bool isCloseToCharacter();
    bool checkPawnCollision(TA_Rect& hitbox) override;
    bool shouldBeDestroyed();
    void destroy() override;

    State state = STATE_IDLE;
    TA_Point velocity;
    bool direction = false;
    float timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
};

#endif
