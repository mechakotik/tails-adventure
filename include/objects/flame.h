#ifndef TA_FLAME_H
#define TA_FLAME_H

#include "object_set.h"

class TA_Flame : public TA_Object {
private:
    const float gravity = 0.125;
    const float maxSpeed = 2;

    float startY, speed;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, float startSpeed);
    bool update() override;
};

class TA_FlameLauncher : public TA_Object {
private:
    const float launchPeriod = 180;

    float timer = 90, startSpeed;
    bool active = false;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, float startSpeed = 3.75);
    bool update() override;
};

#endif // TA_FLAME_H
