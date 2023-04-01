#ifndef TA_ROCK_H
#define TA_ROCK_H

#include "engine/object_set.h"

class TA_Particle : public TA_Object {
private:
    TA_Point velocity, delta;
    double delay = 0, timer = 0;

public:
    using TA_Object::TA_Object;
    void load(std::string filename, TA_Point newPosition, TA_Point newVelocity, TA_Point newDelta, double newDelay = 0);
    bool update() override;
};

#endif // TA_ROCK_H
