#ifndef TA_MINI_SUB_H
#define TA_MINI_SUB_H

#include "object_set.h"

class TA_MiniSub : public TA_Object {
private:
    const double speed = 1.5;

    void updateIdle();
    void updateAttack();

    bool idle = true;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    TA_CollisionType getCollisionType() override {return TA_COLLISION_DAMAGE;}
};

#endif // TA_MINI_SUB_H
