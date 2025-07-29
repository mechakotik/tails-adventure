#ifndef TA_PUSHABLE_OBJECT_H
#define TA_PUSHABLE_OBJECT_H

#include <string>
#include "object_set.h"

class TA_PushableObject : public TA_Object {
private:
    TA_Point velocity;

    const float grv = 0.125;

public:
    using TA_Object::TA_Object;
    void load(std::string filename, TA_Point newPosition);
    bool update() override;
};

class TA_PushableSpring : public TA_PushableObject {
private:
    const float bounceTime = 12;

    TA_Sprite springBounceSprite;

public:
    using TA_PushableObject::TA_PushableObject;
    void load(TA_Point newPosition);
    bool update() override;
    void draw() override;
};

#endif // TA_PUSHABLE_OBJECT_H
