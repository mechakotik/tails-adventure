#ifndef TA_FIRE_H
#define TA_FIRE_H

#include "object_set.h"

class TA_Fire : public TA_Object {
private:
    const double waitTime = 6;
    const double alphaPeriod = 4;

    void updateHitbox();
    void updateAlpha();

    double timer = 0, alphaTimer = 0;
    bool flip;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, bool flip = false);
    bool update() override;
    int getCollisionType() override { return TA_COLLISION_DAMAGE; }
};

#endif // TA_FIRE_H
