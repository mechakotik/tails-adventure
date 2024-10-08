#ifndef TA_MINI_SUB_H
#define TA_MINI_SUB_H

#include "object_set.h"

class TA_MiniSub : public TA_Object {
private:
    const double speed = 1.5;
    const double minExistTime = 240;

    void updateIdle();
    void updateAttack();

    bool idle = true, flip = false;
    double timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    int getCollisionType() override {return TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;}
};

#endif // TA_MINI_SUB_H
