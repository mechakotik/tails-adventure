#ifndef TA_RING_H
#define TA_RING_H

#include "engine/object_set.h"

class TA_Ring : public TA_Object {
private:
    TA_Point velocity{0, -2};
    double timer = 0;

    const int maxTime = 300;
    const double grv = 0.125;
    const double slowdown = 0.75;

public:
    using TA_Object::TA_Object;
    void load(TA_Point newPosition);
    bool update() override;
    bool checkPawnCollision(TA_Polygon &hitbox) override;
};

#endif // TA_RING_H
