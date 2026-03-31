#ifndef TA_SPEEDY2_FIRE_H
#define TA_SPEEDY2_FIRE_H

#include "object_set.h"

class TA_Speedy2Fire : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    int getDrawPriority() override { return 1; }

private:
    float timer = 0;
    float glowTimer = 0;
};

#endif
