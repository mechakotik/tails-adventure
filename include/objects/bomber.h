#ifndef BOMBER_H
#define BOMBER_H

#include "object_set.h"

class TA_Bomber : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(float aimX, float maxY);
    bool update() override;
    void draw() override;
    int getCollisionType() override;

private:
    static constexpr float startYSpeed = 3.2; // TODO: depend on screen height to make it more fair
    static constexpr float gravity = 0.125;
    static constexpr float heightShift = gravity * (startYSpeed / gravity) * (startYSpeed / gravity - 1) / 2;
    static constexpr float attackTime = 12;

    enum class State {
        IDLE,
        FLY,
        PRE_ATTACK,
        POST_ATTACK,
        FLY_AWAY
    };

    void updateIdle();
    void updateFly();
    void updatePreAttack();
    void updatePostAttack();
    void updateFlyAway();

    State state = State::IDLE;
    TA_Point velocity;
    float aimX;
    float maxY;
    float lastCharacterX = 0;
    float timer = 0;
};

#endif // BOMBER_H
