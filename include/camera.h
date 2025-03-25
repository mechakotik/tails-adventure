#ifndef TA_CAMERA_H
#define TA_CAMERA_H

#include "geometry.h"

class TA_Camera {
private:
    const float groundSpeed = 1, airSpeed = 3, springSpeed = 5, maxLockDistance = 256;
    const int shakeRadius = 1, xOffset = 4;
    const float shakeFrequency = 3;

    void updateOffset();

    TA_Point position, lockPosition, shakeDelta;
    TA_Point* followPosition;
    TA_Rect border;

    int yTopOffset, yBottomOffset;
    bool locked = false, lockedX = false, lockedY = false;
    float shakeTime = -1;

public:
    void update(bool ground, bool spring);
    void setFollowPosition(TA_Point* newFollowPosition);
    void setLockPosition(TA_Point newLockPosition);
    void forceLockX();
    bool isLocked() { return locked && lockedX && lockedY; }
    void unlock() { locked = lockedX = lockedY = false; }
    void setBorder(TA_Rect newBorder) { border = newBorder; }
    void shake(float time) { shakeTime = time; }
    TA_Point getPosition() { return position + shakeDelta; }
    TA_Point getRelative(TA_Point realPosition) { return realPosition - (position + shakeDelta); }
};

#endif // TA_CAMERA_H
