#ifndef TA_PAWN_H
#define TA_PAWN_H

#include "sprite.h"
#include "geometry.h"

enum TA_COLLISION_STATE {
    TA_GROUND_COLLISION = (1 << 0),
    TA_WALL_COLLISION = (1 << 1),
    TA_CEIL_COLLISION = (1 << 2),
    TA_COLLISION_ERROR = (1 << 3)
};

class TA_Pawn : public TA_Sprite {
protected:
    TA_Point position;
    int getCollisionFlags(TA_Point topLeft, TA_Point bottomRight);

public:
    virtual bool checkPawnCollision(TA_Polygon &hitbox) {return false;}
    int moveAndCollide(TA_Point topLeft, TA_Point bottomRight, TA_Point velocity, bool ground = false);
};

#endif // TA_PAWN_H
