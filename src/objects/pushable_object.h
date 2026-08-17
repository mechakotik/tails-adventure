#ifndef TA_PUSHABLE_OBJECT_H
#define TA_PUSHABLE_OBJECT_H

#include <string>
#include "object_set.h"

class TA_PushableObject : public TA_Object {
private:
    enum class GloveState { NORMAL, CARRIED, THROWN };

    TA_Point velocity;
    GloveState gloveState = GloveState::NORMAL;

    const float grv = 0.125;

    bool updateThrown();

protected:
    virtual void setCollisionEnabled(bool enabled);

public:
    using TA_Object::TA_Object;
    void load(std::string filename, TA_Point newPosition);
    bool update() override;
    int getDrawPriority() override { return 1; }
    TA_PushableObject* asPushableObject() override { return this; }
    bool canBePickedUp() const { return gloveState == GloveState::NORMAL; }
    bool isCarried() const { return gloveState == GloveState::CARRIED; }
    bool isThrown() const { return gloveState == GloveState::THROWN; }
    void startCarry();
    void setCarriedPosition(TA_Point newPosition);
    void throwFromCarry(TA_Point throwVelocity);
    void releaseCarry();
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

protected:
    void setCollisionEnabled(bool enabled) override;
};

#endif // TA_PUSHABLE_OBJECT_H
