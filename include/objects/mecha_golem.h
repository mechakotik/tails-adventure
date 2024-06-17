#ifndef TA_MECHA_GOLEM_H
#define TA_MECHA_GOLEM_H

#include "object_set.h"

class TA_MechaGolem : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load();
    bool update() override;
    void draw() override;

private:
    enum State {
        STATE_IDLE,
        STATE_P1_WAIT,
    };

    State state = STATE_IDLE;

    void updateIdle();
    void updateP1Wait();

    void updateHitboxes();
    void insertBorderHitboxes();
};

#endif
