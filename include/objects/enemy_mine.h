#ifndef TA_MINE_H
#define TA_MINE_H

#include "object_set.h"

class TA_EnemyMine : public TA_Object {
private:
    const float interval = 50;

    TA_Point startPosition;
    float timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
};

#endif // TA_MINE_H
