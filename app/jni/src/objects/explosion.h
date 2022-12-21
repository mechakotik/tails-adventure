#ifndef TA_EXPLOSION_H
#define TA_EXPLOSION_H

#include "engine/object_set.h"
#include "engine/geometry.h"

class TA_Explosion : public TA_Object {
private:
    double timer = 0;
    int delay = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, int newDelay = 0);
    bool update() override;
    void draw() override;
};

#endif // TA_EXPLOSION_H
