#ifndef TA_BIRD_WALKER_H
#define TA_BIRD_WALKER_H

#include "engine/object_set.h"

enum TA_BirdWalkerState {
    TA_BIRD_WALKER_STATE_IDLE,
    TA_BIRD_WALKER_STATE_GROUND,
    TA_BIRD_WALKER_STATE_AIR
};

class TA_BirdWalker : public TA_Object {
private:
    TA_BirdWalkerState state = TA_BIRD_WALKER_STATE_IDLE;
    double floorY;

public:
    using TA_Object::TA_Object;
    void load(double newFloorY);
    bool update() override;
};

#endif // TA_BIRD_WALKER_H
