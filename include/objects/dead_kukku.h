#ifndef TA_DEAD_KUKKU_H
#define TA_DEAD_KUKKU_H

#include "object_set.h"

class TA_DeadKukku : public TA_Object {
private:
    TA_Point velocity{0, -2.5};
    float timer = 0;

    const float grv = 0.125;
    const float deathTime = 500;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    int getCollisionType() override { return TA_COLLISION_TRANSPARENT; }
};

#endif // TA_DEAD_KUKKU_H
