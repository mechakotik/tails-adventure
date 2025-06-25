#ifndef TA_RING_H
#define TA_RING_H

#include "object_set.h"
#include "sound.h"

class TA_Ring : public TA_Object {
private:
    TA_Point velocity;
    TA_Sound ringSound;
    float timer = 0;
    float delay = 0;
    bool stationary = false;
    bool collected = false;

    static constexpr int maxTime = 300;
    static constexpr float grv = 0.125;
    static constexpr float waterGrv = 0.05;
    static constexpr float slowdown = 0.75;
    static constexpr float drag = 0.005;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, TA_Point velocity, float delay = 0);
    void load(TA_Point position, float startSpeed = -2);
    void loadStationary(TA_Point position);
    bool update() override;
    int getDrawPriority() override { return 1; }
};

#endif // TA_RING_H
