#ifndef TA_BOMB_H
#define TA_BOMB_H

#include "engine/object_set.h"
#include "engine/geometry.h"

class TA_Bomb : public TA_Object {
private:
    TA_Point velocity;
    bool direction;
    double timer = 0;

    const TA_Point startVelocity = {2, -1};
    const double grv = 0.125;

public:
    using TA_Object::TA_Object;
    void load(TA_Point newPosition, bool newDirection);
    bool checkPawnCollision(TA_Polygon hitbox) override;
    bool update() override;
};

#endif // TA_BOMB_H
