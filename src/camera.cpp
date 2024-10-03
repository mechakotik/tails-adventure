#include <algorithm>
#include "camera.h"
#include "tools.h"

void TA_Camera::setFollowPosition(TA_Point *newFollowPosition)
{
    updateOffset();
    followPosition = newFollowPosition;

    if(!lockedX) {
        position.x = (*followPosition).x;
    }
    if(!lockedY) {
        position.y = (*followPosition).y - yBottomOffset;
    }
}

void TA_Camera::setLockPosition(TA_Point newLockPosition)
{
    lockPosition = newLockPosition;
    locked = true;
    lockedX = lockedY = false;
}

void TA_Camera::forceLockX()
{
    position.x = lockPosition.x;
    lockedX = true;
}

void TA_Camera::update(bool ground, bool spring)
{
    updateOffset();
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

    TA_Point previousPosition = position, watchPosition = *followPosition;
    bool offset = true;
    if(locked && (lockedX || lockedY) && lockPosition.getDistance(position) < 64) {
        watchPosition = lockPosition;
        offset = false;
    }

    if(!lockedX) {
        if(position.x < watchPosition.x - (offset ? xOffset : 0)) {
            position.x = move(position.x, watchPosition.x - (offset ? xOffset : 0));
        }
        else if(position.x > watchPosition.x + (offset ? xOffset : 0)) {
            position.x = move(position.x, watchPosition.x + (offset ? xOffset : 0));
        }
    }
    if(!lockedY) {
        if(position.y < watchPosition.y - (offset ? yBottomOffset : 0)) {
            position.y = move(position.y, watchPosition.y  - (offset ? yBottomOffset : 0));
        }
        else if(position.y > watchPosition.y + (offset ? yTopOffset : 0)) {
            position.y = move(position.y, watchPosition.y + (offset ? yTopOffset : 0));
        }
    }

    if(locked && lockPosition.getDistance(position) <= maxLockDistance) {
        if(!lockedX && (TA::sign(previousPosition.x - lockPosition.x) != TA::sign(position.x - lockPosition.x) || abs(position.x - lockPosition.x) < 0.5)) {
            position.x = lockPosition.x;
            lockedX = true;
        }
        if(!lockedY && (TA::sign(previousPosition.y - lockPosition.y) != TA::sign(position.y - lockPosition.y) || abs(position.y - lockPosition.y) < 0.5)) {
            position.y = lockPosition.y;
            lockedY = true;
        }
    }

    auto normalize = [&] (double current, double left, double right) {
        current = std::max(current, left);
        current = std::min(current, right);
        return current;
    };

    position.x = normalize(position.x, border.topLeft.x, border.bottomRight.x - TA::screenWidth);
    position.y = normalize(position.y, border.topLeft.y, border.bottomRight.y - TA::screenHeight);

    if(shakeTime > 0) {
        int previousStep = shakeTime / shakeFrequency;
        shakeTime -= TA::elapsedTime;
        int currentStep = shakeTime / shakeFrequency;
        if(previousStep != currentStep) {
            shakeDelta.x = (TA::random::next() % 2 == 0 ? -shakeRadius : shakeRadius);
            shakeDelta.y = (TA::random::next() % 2 == 0 ? -shakeRadius : shakeRadius);
        }
    }
    else {
        shakeDelta = {0, 0};
    }
}

void TA_Camera::updateOffset()
{
    if(TA::levelPath == "maps/pm/pm4") {
        yTopOffset = -8;
        yBottomOffset = 16;
    }
    else {
        yTopOffset = 8;
        yBottomOffset = 16;
    }
}
