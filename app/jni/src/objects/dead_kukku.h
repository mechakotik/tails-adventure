#ifndef TA_DEAD_KUKKU_H
#define TA_DEAD_KUKKU_H

#include "engine/object_set.h"

class TA_DeadKukku : public TA_Object {
private:
    TA_Point velocity{0, -2.5};
    double timer = 0;

    const double grv = 0.125;
    const double deathTime = 500;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update();
    TA_CollisionType getCollisionType() override {return TA_COLLISION_TRANSPARENT;}
};

#endif // TA_DEAD_KUKKU_H
