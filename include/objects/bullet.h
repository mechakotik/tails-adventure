#ifndef TA_BULLET_H
#define TA_BULLET_H

#include "object_set.h"

class TA_Bullet : public TA_Object {
private:
    TA_Point velocity;

protected:
    virtual int getCollisionFlags() {return TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP;}
    virtual void onDestroy() {}

public:
    using TA_Object::TA_Object;
    void load(std::string filename, TA_Point newPosition, TA_Point newVelocity);
    bool update() override;
    int getCollisionType() override {return TA_COLLISION_DAMAGE;}
    int getDrawPriority() override {return 1;}
};

class TA_BirdWalkerBullet : public TA_Bullet {
public:
    using TA_Bullet::TA_Bullet;
    void load(TA_Point newPosition, TA_Point newVelocity);
    bool update() override;
};

class TA_VulcanGunBullet : public TA_Bullet {
private:
    const double existTime = 15;

    int getCollisionFlags() override {return TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP | TA_COLLISION_DAMAGE;}

    TA_Sound explosionSound;
    double timer = 0;

public:
    using TA_Bullet::TA_Bullet;
    void load(TA_Point position, TA_Point velocity);
    bool update() override;
    void onDestroy() override;
    int getCollisionType() override {return TA_COLLISION_ATTACK;}
};

#endif // TA_BULLET_H
