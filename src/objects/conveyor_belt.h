#ifndef TA_CONVEYOR_BELT_H
#define TA_CONVEYOR_BELT_H

#include "object_set.h"

class TA_ConveyorBelt : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point topLeft, TA_Point bottomRight, bool flip);
    bool update() { return true; }
};

#endif // TA_CONVEYOR_BELT_H
