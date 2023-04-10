#ifndef TA_BREAKABLE_BLOCK_H
#define TA_BREAKABLE_BLOCK_H

#include "object_set.h"

class TA_BreakableBlock : public TA_Object {
private:
    const double grv = 0.25;
    bool dropsRing;

public:
    using TA_Object::TA_Object;
    void load(TA_Point newPosition, bool newDropsRing);
    bool update() override;
    TA_CollisionType getCollisionType() override {return TA_COLLISION_SOLID;}
    int getDrawPriority() override {return 1;}
};

#endif // TA_BREAKABLE_BLOCK_H
