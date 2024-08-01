#ifndef TA_NAPALM_FIRE_H
#define TA_NAPALM_FIRE_H

#include "object_set.h"

class TA_NapalmFire : public TA_Object {
private:
    TA_Point velocity, topLeft{0, 23}, bottomRight{8, 31};

    bool checkPawnCollision(TA_Polygon &hitbox) override;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, double xsp);
    bool update() override;
};

#endif // TA_NAPALM_FIRE_H
