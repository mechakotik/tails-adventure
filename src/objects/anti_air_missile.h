#ifndef TA_ANTI_AIR_MISSILE_H
#define TA_ANTI_AIR_MISSILE_H

#include "object_set.h"

class TA_AntiAirMissile : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;

private:
    static constexpr float explosionSpeed = 1.5;

    TA_Point velocity;
    TA_Sound explosionSound;
    bool destroyed = false;
    float timer = 0;
};

#endif // TA_ANTI_AIR_MISSILE_H
