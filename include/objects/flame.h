#ifndef TA_FLAME_H
#define TA_FLAME_H

#include "object_set.h"

class TA_Flame : public TA_Object {
private:
    const double gravity = 0.125;
    const double maxSpeed = 2;

    double startY, speed;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, double startSpeed);
    bool update() override;
    TA_CollisionType getCollisionType() override {return TA_COLLISION_DAMAGE;}
};

class TA_FlameLauncher : public TA_Object {
private:
    const double launchPeriod = 180;

    double timer = 90, startSpeed;
    bool active = false;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, double startSpeed = 3.75);
    bool update() override;
};

#endif // TA_FLAME_H
