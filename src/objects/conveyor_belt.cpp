#include "conveyor_belt.h"

void TA_ConveyorBelt::load(TA_Point topLeft, TA_Point bottomRight, bool direction) {
    hitbox.setRectangle(topLeft, bottomRight);
    collisionType = (direction ? TA_COLLISION_CONVEYOR_BELT_RIGHT : TA_COLLISION_CONVEYOR_BELT_LEFT);
}
