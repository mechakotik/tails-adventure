#ifndef TA_BEEHIVE_H
#define TA_BEEHIVE_H

#include "geometry.h"
#include "object_set.h"

class TA_BeeHive : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    int getCollisionType() override;
    void deleteBee() { beeCount--; }

private:
    static constexpr double cooldownTime = 40;

    bool destroyed = false;
    int beeCount = 0;
    double timer = 0;
};

class TA_Bee : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position, TA_BeeHive* hive);
    bool update() override;
    int getCollisionType() override { return TA_COLLISION_DAMAGE | TA_COLLISION_TARGET; }

private:
    static constexpr double flySpeed = 1;
    static constexpr double circleRadius = 12;

    void updateFlyDown();
    void updateNormal();

    TA_BeeHive* hive;

    bool flyDown = true;
    double flyDownY = 0;

    TA_Point leftCircleCenter, rightCircleCenter;
    bool leftCircle = false, direction = false;
    double timer = 0;
};

class TA_StrongBee : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    int getCollisionType() override { return TA_COLLISION_DAMAGE | TA_COLLISION_TARGET; }

private:
    static constexpr double idleTime = 40;
    static constexpr double speed = 0.4;
    static constexpr double rotateSpeed = 0.25;

    double getNeedeedAngle();

    double timer = 0;
    double angle = 0;
};

#endif // TA_BEEHIVE_H
