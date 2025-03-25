#ifndef TA_MOVING_PLATFORM_H
#define TA_MOVING_PLATFORM_H

#include "object_set.h"

class TA_MovingPlatform : public TA_Object {
private:
    const float speed = 0.5;

    void updateIdle();
    void movePlatform();
    void moveCharacter();

    bool characterIsOnTop();
    bool moveByX();
    bool exceedsBorder();

    TA_Point startPosition, endPosition, prevPosition;
    bool idle = true, reverse = false;

public:
    using TA_Object::TA_Object;
    void load(TA_Point startPosition, TA_Point endPosition, bool idle = true);
    bool update() override;
    int getCollisionType() override;
};

#endif // TA_MOVING_PLATFORM_H
