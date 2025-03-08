#include "conveyor_belt.h"

void TA_ConveyorBelt::load(TA_Point topLeft, TA_Point bottomRight, bool flip) {
    hitbox.setRectangle(topLeft, bottomRight);
    collisionType = (flip ? TA_COLLISION_CONVEYOR_BELT_LEFT : TA_COLLISION_CONVEYOR_BELT_RIGHT);
}
