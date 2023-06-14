#include "object_set.h"

class TA_Flame : public TA_Object {
private:
    const double startSpeed = -3.5;
    const double gravity = 0.125;

    TA_Point velocity{0, startSpeed};
    double startY;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    TA_CollisionType getCollisionType() override {return TA_COLLISION_DAMAGE;}
};

class TA_FlameLauncher : public TA_Object {
private:
    const double launchPeriod = 180;

    double timer = 90;
    bool active = false;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
};
