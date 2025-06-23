#ifndef TA_BEEHIVE_H
#define TA_BEEHIVE_H

#include "geometry.h"
#include "object_set.h"

class TA_BeeHive : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    void deleteBee() { beeCount--; }

private:
    static constexpr float cooldownTime = 40;

    bool destroyed = false;
    int beeCount = 0;
    float timer = 0;
};

class TA_Bee : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position, TA_BeeHive* hive);
    bool update() override;

private:
    static constexpr float flySpeed = 1;
    static constexpr float circleRadius = 12;

    void updateFlyDown();
    void updateNormal();

    TA_BeeHive* hive;

    bool flyDown = true;
    float flyDownY = 0;

    TA_Point leftCircleCenter, rightCircleCenter;
    bool leftCircle = false, direction = false;
    float timer = 0;
};

class TA_StrongBee : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;

private:
    static constexpr float idleTime = 40;
    static constexpr float speed = 1;
    static constexpr float rotateSpeed = 0.1;

    float getNeedeedAngle();

    float timer = 0;
    float angle = 0;
};

#endif // TA_BEEHIVE_H
