#ifndef TA_BOMB_H
#define TA_BOMB_H

#include "object_set.h"
#include "geometry.h"
#include "sound.h"

// TODO: clean up this code

enum TA_BombMode : int {
    TA_BOMB_MODE_DEFAULT,
    TA_BOMB_MODE_AIR,
    TA_BOMB_MODE_CROUCH,
    TA_BOMB_MODE_HELITAIL
};

class TA_Bomb : public TA_Object {
private:
    const double grv = 0.125;
    const double destroyedTime = 5;

    TA_Sound explosionSound;
    TA_BombMode mode;

    bool direction, ground = false, destroyed = false;
    double timer = 0;

protected:
    virtual bool shouldExplode();
    virtual void explode();

    double speed = 1.15, crouchThrowHeight = -6;
    TA_Point topLeft{3, 4}, bottomRight{12, 14};
    TA_Point startVelocity = {2 * speed, -1 * speed};
    TA_Point startCrouchVelocity = {1.35 * speed, -0.7 * speed};
    TA_Point startHelitailVelocity = {0, 0};
    TA_Point velocity;

public:
    using TA_Object::TA_Object;
    virtual void load(TA_Point newPosition, bool newDirection, TA_BombMode mode);
    bool update() override;
    void draw() override;

    bool checkPawnCollision(TA_Polygon &hitbox) override;
    int getCollisionType() override {return TA_COLLISION_BOMB;}
    int getDrawPriority() override {return 1;}
};

class TA_RemoteBomb : public TA_Bomb {
private:
    const double friction = 0.0125;

    bool shouldExplode() override;

public:
    using TA_Bomb::TA_Bomb;
    void load(TA_Point newPosition, bool newDirection, TA_BombMode mode) override;
    bool update() override;
};

class TA_NapalmBomb : public TA_Bomb {
private:
    bool shouldExplode() override;
    void explode() override;

public:
    using TA_Bomb::TA_Bomb;
    void load(TA_Point newPosition, bool newDirection, TA_BombMode mode) override;
    bool update() override;
    int getCollisionType() override {return TA_COLLISION_TRANSPARENT;}
};

#endif // TA_BOMB_H
