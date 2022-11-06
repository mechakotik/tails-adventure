#ifndef TA_PAWN_H
#define TA_PAWN_H

#include "sprite.h"
#include "geometry.h"

class TA_Pawn : public TA_Sprite {
protected:
    TA_Point position;

public:
    virtual bool checkCollision(TA_Polygon hitbox) {return false;}
    void moveAndCollide(TA_Polygon xHitbox, TA_Polygon yHitbox, TA_Point velocity, bool ground);
};

#endif // TA_PAWN_H
