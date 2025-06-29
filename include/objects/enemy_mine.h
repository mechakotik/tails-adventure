#ifndef TA_MINE_H
#define TA_MINE_H

#include "object_set.h"

class TA_EnemyMine : public TA_Object {
private:
    static constexpr float interval = 50;
    static constexpr float gravity = 0.075;
    static constexpr float maxYSpeed = 2;

    TA_Point startPosition;
    float timer = 0;
    float ysp = 0;
    bool fall = false;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, bool fall);
    bool update() override;
};

#endif // TA_MINE_H
