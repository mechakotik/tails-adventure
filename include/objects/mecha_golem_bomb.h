#ifndef TA_MECHA_GOLEM_BOMB_H
#define TA_MECHA_GOLEM_BOMB_H

#include "object_set.h"

class TA_MechaGolemBomb : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    TA_CollisionType getCollisionType() override {return TA_COLLISION_DAMAGE;}

private:
    const double gravity = 0.125;
    const double maxSpeed = 2.5;

    double speed = 0;
};

#endif
