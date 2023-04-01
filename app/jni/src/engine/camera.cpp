#include <algorithm>
#include "camera.h"
#include "tools.h"

void TA_Camera::setFollowPosition(TA_Point *newFollowPosition)
{
    followPosition = newFollowPosition;
    position = *followPosition;
}

void TA_Camera::setLockPosition(TA_Point newLockPosition)
{
    lockPosition = newLockPosition;
    locked = true;
    lockedX = lockedY = false;
}

void TA_Camera::update(bool ground, bool spring)
{
    double movementSpeed = airSpeed;
    if(ground) {
        movementSpeed = groundSpeed;
    }
    if(spring) {
        movementSpeed = springSpeed;
    }

    auto move = [&] (double current, double need) {
        if(current < need) {
            current = std::min(need, current + movementSpeed * TA::elapsedTime);
        }
        else {
            current = std::max(need, current - movementSpeed * TA::elapsedTime);
        }
        return current;
    };

    TA_Point previousPosition = position;
    if(!lockedX) {
        position.x = move(position.x, followPosition->x);
    }
    if(!lockedY) {
        position.y = move(position.y, followPosition->y);
    }
    if(locked) {
        if(!lockedY && lockPosition.getDistance(position) <= 256 && TA::sign(previousPosition.y - lockPosition.y) != TA::sign(position.y - lockPosition.y)) {
            position.y = lockPosition.y;
            lockedY = true;
        }
        if(lockedY && !lockedX && TA::sign(previousPosition.x - lockPosition.x) != TA::sign(position.x - lockPosition.x)) {
            position.x = lockPosition.x;
            lockedX = true;
        }
    }

    auto normalize = [&] (double current, double left, double right) {
        current = std::max(current, left);
        current = std::min(current, right);
        return current;
    };

    position.x = normalize(position.x, border.topLeft.x, border.bottomRight.x);
    position.y = normalize(position.y, border.topLeft.y, border.bottomRight.y);
}
