#ifndef TA_DRILL_MOLE_H
#define TA_DRILL_MOLE_H

#include "object_set.h"

class TA_DrillMole : public TA_Object {
private:
    const float idleTime = 120;
    const float moveTime = 30;
    const float moveRange = 16;

    enum State { STATE_IDLE_UP, STATE_MOVE_DOWN, STATE_IDLE_DOWN, STATE_MOVE_UP };

    std::pair<State, float> getStateAndTime();
    void updatePosition() override;
    bool shouldBeDestroyed();
    void destroy() override;

    float startY;
    float timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
};

#endif // TA_DRILL_MOLE_H
