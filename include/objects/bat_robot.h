#ifndef TA_BAT_ROBOT_H
#define TA_BAT_ROBOT_H

#include "object_set.h"

class TA_BatRobot : public TA_Object {
private:
    enum State { STATE_IDLE, STATE_ACTIVE, STATE_ATTACK };

    const float speed = 0.7;
    const float gravity = -0.1 * speed * speed;
    const float cooldownTime = 48;

    float findOptimalSpeed(float deltaY);

    TA_Point velocity;
    State state = STATE_IDLE;
    float timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point newPosition);
    bool update() override;
    int getCollisionType() override { return TA_COLLISION_DAMAGE | TA_COLLISION_TARGET; }
    bool checkPawnCollision(TA_Polygon& collisionHitbox) override;
};

#endif // TA_BAT_ROBOT_H
