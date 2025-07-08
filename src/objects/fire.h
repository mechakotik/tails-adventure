#ifndef TA_FIRE_H
#define TA_FIRE_H

#include "object_set.h"

class TA_Fire : public TA_Object {
private:
    static constexpr float waitTime = 6;
    static constexpr float alphaPeriod = 4;

    void updateHitbox();
    void updateAlpha();

    float timer = 0, alphaTimer = 0;
    bool flip;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, bool flip = false);
    bool update() override;
};

#endif // TA_FIRE_H
