#ifndef TA_BULLET_H
#define TA_BULLET_H

#include "object_set.h"

class TA_Bullet : public TA_Object {
private:
    TA_Point velocity;

public:
    using TA_Object::TA_Object;
    void load(std::string filename, TA_Point newPosition, TA_Point newVelocity, int frameWidth = -1, int frameHeight = -1);
    bool update() override;
    TA_CollisionType getCollisionType() override {return TA_COLLISION_DAMAGE;}
    int getDrawPriority() override {return 1;}
};

class TA_BirdWalkerBullet : public TA_Bullet {
public:
    using TA_Bullet::TA_Bullet;
    void load(TA_Point newPosition, TA_Point newVelocity);
    bool update() override;
};

#endif // TA_BULLET_H
