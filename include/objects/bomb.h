#ifndef TA_BOMB_H
#define TA_BOMB_H

#include "object_set.h"
#include "geometry.h"
#include "sound.h"

enum TA_BombMode : int {
    TA_BOMB_MODE_DEFAULT,
    TA_BOMB_MODE_AIR,
    TA_BOMB_MODE_CROUCH,
    TA_BOMB_MODE_HELITAIL
};

class TA_Bomb : public TA_Object {
private:
    const double grv = 0.125;

    TA_Sound explosionSound;
    TA_BombMode mode;

    bool direction;
    double timer = 0;

protected:
    virtual bool shouldExplode() {return false;}

    double speed = 1.15;
    int destroyFlags = TA_COLLISION_SOLID | TA_COLLISION_HALF_SOLID | TA_COLLISION_DAMAGE | TA_COLLISION_PUSHABLE;

    TA_Point topLeft{3, 4}, bottomRight{12, 14};
    TA_Point startVelocity = {2 * speed, -1 * speed};
    TA_Point startCrouchVelocity = {1.35 * speed, -0.7 * speed};
    TA_Point startHelitailVelocity = {0, 0};
    TA_Point velocity;

public:
    using TA_Object::TA_Object;
    virtual void load(TA_Point newPosition, bool newDirection, TA_BombMode mode);
    bool checkPawnCollision(TA_Polygon &hitbox) override;
    bool update() override;
    TA_CollisionType getCollisionType() override {return TA_COLLISION_BOMB;}
    int getDrawPriority() override {return 1;}
};

class TA_RemoteBomb : public TA_Bomb {
private:
    const double friction = 0.0125;
    const double waitTime = 20;

    double timer = 0;

protected:
    bool shouldExplode() override;

public:
    using TA_Bomb::TA_Bomb;
    void load(TA_Point newPosition, bool newDirection, TA_BombMode mode) override;
    bool update() override;
};

#endif // TA_BOMB_H
