#ifndef TA_CONVEYOR_BELT_H
#define TA_CONVEYOR_BELT_H

#include "object_set.h"

class TA_ConveyorBelt : public TA_Object {
private:
    TA_CollisionType collisionType = TA_COLLISION_CONVEYOR_BELT_LEFT;

public:
    using TA_Object::TA_Object;
    void load(TA_Point topLeft, TA_Point bottomRight, bool direction);
    bool update() {return true;}
    int getCollisionType() {return collisionType;}
};

#endif // TA_CONVEYOR_BELT_H
