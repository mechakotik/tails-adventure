#ifndef TA_DRILL_MOLE_H
#define TA_DRILL_MOLE_H

#include "object_set.h"

class TA_DrillMole : public TA_Object {
private:
    const double idleTime = 120;
    const double moveTime = 30;
    const double moveRange = 16;

    enum State {
        STATE_IDLE_UP,
        STATE_MOVE_DOWN,
        STATE_IDLE_DOWN,
        STATE_MOVE_UP
    };

    std::pair<State, double> getStateAndTime();
    void updatePosition() override;
    bool shouldBeDestroyed();
    void destroy();

    double startY;
    double timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    TA_CollisionType getCollisionType() override {return TA_COLLISION_DAMAGE;}
};

#endif // TA_DRILL_MOLE_H
