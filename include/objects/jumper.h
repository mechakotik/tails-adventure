#ifndef TA_JUMPER_H
#define TA_JUMPER_H

#include "object_set.h"

class TA_Jumper : public TA_Object {
private:
    enum State {
        STATE_IDLE,
        STATE_AIM,
        STATE_JUMP
    };

    void updateIdle();
    void updateAim();
    void updateJump();
    void updateDirection();
    bool shouldBeDestroyed();
    void destroy();

    State state = STATE_IDLE;
    bool direction = false;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    TA_CollisionType getCollisionType() override {return TA_COLLISION_DAMAGE;}
};

#endif
