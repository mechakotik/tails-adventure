#ifndef TA_BREAKABLE_BLOCK_H
#define TA_BREAKABLE_BLOCK_H

#include "engine/object_set.h"

class TA_BreakableBlock : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point newPosition);
    bool update() override;
    TA_CollisionType getCollisionType() override {return TA_COLLISION_SOLID;}
};

#endif // TA_BREAKABLE_BLOCK_H
