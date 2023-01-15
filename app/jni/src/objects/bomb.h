#ifndef TA_BOMB_H
#define TA_BOMB_H

#include "engine/object_set.h"
#include "engine/geometry.h"
#include "engine/sound.h"

enum TA_BombMode : int {
    TA_BOMB_MODE_DEFAULT,
    TA_BOMB_MODE_AIR,
    TA_BOMB_MODE_CROUCH,
    TA_BOMB_MODE_HELITAIL
};

class TA_Bomb : public TA_Object {
private:
    TA_Point velocity;
    TA_Sound explosionSound;
    bool direction;
    double timer = 0;

    const TA_Point startVelocity = {2, -1};
    const TA_Point startCrouchVelocity = {1.35, -1};
    const TA_Point startHelitailVelocity = {0, 0};
    const double grv = 0.125;

public:
    using TA_Object::TA_Object;
    void load(TA_Point newPosition, bool newDirection, TA_BombMode mode);
    bool checkPawnCollision(TA_Polygon hitbox) override;
    bool update() override;
    int getDrawPriority() override {return 1;}
};

#endif // TA_BOMB_H
