#ifndef TA_ANTI_AIR_MISSILE_H
#define TA_ANTI_AIR_MISSILE_H

#include "object_set.h"

class TA_AntiAirMissile : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    int getCollisionType() override {return TA_COLLISION_ATTACK | TA_COLLISION_BOMB | TA_COLLISION_DRILL;}

private:
    TA_Point velocity;
    TA_Sound explosionSound;
    bool destroyed = false;
    double timer = 0;
};

#endif // TA_ANTI_AIR_MISSILE_H
