#ifndef TA_MINE_H
#define TA_MINE_H

#include "object_set.h"

class TA_EnemyMine : public TA_Object {
private:
    const double interval = 50;

    TA_Point startPosition;
    double timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    int getCollisionType() override {return TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;}
};

#endif // TA_MINE_H
