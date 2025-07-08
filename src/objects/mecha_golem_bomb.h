#ifndef TA_MECHA_GOLEM_BOMB_H
#define TA_MECHA_GOLEM_BOMB_H

#include "object_set.h"

class TA_MechaGolemBomb : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;

private:
    const float gravity = 0.125;
    const float maxSpeed = 2.5;

    float speed = 0;
};

#endif
