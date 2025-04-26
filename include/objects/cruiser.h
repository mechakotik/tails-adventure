#ifndef TA_CRUISER_H
#define TA_CRUISER_H

#include "object_set.h"

class TA_Cruiser : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load();
    bool update() override;
    int getCollisionType() override { return TA_COLLISION_DAMAGE | TA_COLLISION_TARGET; }
    void draw() override;

private:
    enum class State { IDLE, ACTIVE, DESTROYED, UNLOCK };

    void updateIdle();
    void updateActive();
    void updateDestroyed();
    void updateUnlock();
    void updateBorderPosition();
    void updateBirds();
    void updateDamage();

    State state = State::IDLE;
    TA_Point lockPosition;
    bool cameraNormalized = false;

    TA_Sprite watcherSprite;
    TA_Sprite leftThrowerSprite;
    TA_Sprite rightThrowerSprite;
    int leftThrowerPrevFrame = 1;
    int rightThrowerPrevFrame = 1;

    TA_Sound hitSound;

    int health = 48;
    float speed = 1.5;
};

#endif // TA_CRUISER_H
