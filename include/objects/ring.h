#ifndef TA_RING_H
#define TA_RING_H

#include "object_set.h"
#include "sound.h"

class TA_Ring : public TA_Object {
private:
    TA_Point velocity;
    TA_Sound ringSound;
    double timer = 0;

    const int maxTime = 300;
    const double grv = 0.125;
    const double slowdown = 0.75;

public:
    using TA_Object::TA_Object;
    void load(TA_Point newPosition, double startSpeed = -2);
    bool update() override;
    bool checkPawnCollision(TA_Polygon &hitbox) override;
    int getDrawPriority() override {return 1;}
};

#endif // TA_RING_H
