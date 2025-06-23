#ifndef TA_BARREL_H
#define TA_BARREL_H

#include "object_set.h"

class TA_Barrel : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position, TA_Point velocity);
    bool update() override;

private:
    TA_Point velocity;
};

#endif // TA_BARREL_H
