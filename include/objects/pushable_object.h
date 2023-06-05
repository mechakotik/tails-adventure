#ifndef TA_PUSHABLE_OBJECT_H
#define TA_PUSHABLE_OBJECT_H

#include <string>
#include "object_set.h"

class TA_PushableObject : public TA_Object {
private:
    TA_Point velocity;

    const double grv = 0.125;

public:
    using TA_Object::TA_Object;
    virtual void load(std::string filename, TA_Point newPosition);
    bool update();
    TA_CollisionType getCollisionType() {return TA_COLLISION_PUSHABLE;}
    bool checkPawnCollision(TA_Polygon &hitbox) override;
};

class TA_PushableRock : public TA_PushableObject {
public:
    using TA_PushableObject::TA_PushableObject;
    void load(TA_Point newPosition) {TA_PushableObject::load("objects/rock.png", newPosition);}
};

class TA_PushableSpring : public TA_PushableObject {
public:
    using TA_PushableObject::TA_PushableObject;
    void load(TA_Point newPosition);
};

#endif // TA_PUSHABLE_OBJECT_H
