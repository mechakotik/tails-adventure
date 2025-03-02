#ifndef TA_RING_H
#define TA_RING_H

#include "object_set.h"
#include "sound.h"

class TA_Ring : public TA_Object {
private:
    TA_Point velocity;
    TA_Sound ringSound;
    double timer = 0;
    double delay = 0;
    bool stationary = false;
    bool collected = false;

    static constexpr int maxTime = 300;
    static constexpr double grv = 0.125;
    static constexpr double waterGrv = 0.05;
    static constexpr double slowdown = 0.75;
    static constexpr double drag = 0.005;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, TA_Point velocity, double delay = 0);
    void load(TA_Point position, double startSpeed = -2);
    void loadStationary(TA_Point position);
    bool update() override;
    bool checkPawnCollision(TA_Polygon& hitbox) override;
    int getDrawPriority() override { return 1; }
};

#endif // TA_RING_H
