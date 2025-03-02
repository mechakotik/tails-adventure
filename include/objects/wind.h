#ifndef TA_WIND_H
#define TA_WIND_H

#include "object_set.h"

class TA_Wind : public TA_Object {
private:
    const double leafSpawnTime = 5;

    enum Side { SIDE_UP, SIDE_DOWN, SIDE_LEFT, SIDE_RIGHT };

    void spawnLeaf();
    Side getRandomLeafSide();
    TA_Point getRandomPointOnSide(Side side);

    TA_Point velocity;
    double timer = 0;

protected:
    virtual bool shouldBlow();

public:
    using TA_Object::TA_Object;
    void load(TA_Point topLeft, TA_Point bottomRight, TA_Point velocity);
    bool update();
};

class TA_StrongWind : public TA_Wind {
private:
    bool blowing = false;

protected:
    bool shouldBlow() override;

public:
    using TA_Wind::TA_Wind;
    void load(TA_Point topLeft, TA_Point bottomRight);
};

class TA_Leaf : public TA_Object {
private:
    const double existTime = 200;

    TA_Point velocity;
    double timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, TA_Point velocity);
    bool update();
};

#endif // TA_WIND_H
