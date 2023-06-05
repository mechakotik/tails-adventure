#ifndef TA_CAMERA_H
#define TA_CAMERA_H

#include "geometry.h"

class TA_Camera {
private:
    const double groundSpeed = 1, airSpeed = 3, springSpeed = 5, maxLockDistance = 256;
    const int shakeRadius = 1, xOffset = 4, yTopOffset = 8, yBottomOffset = 16;
    const double shakeFrequency = 3;

    TA_Point position, lockPosition, shakeDelta;
    TA_Point *followPosition;
    TA_Rect border;

    bool locked = false, lockedX = false, lockedY = false;
    double shakeTime = -1;

public:
    void update(bool ground, bool spring);
    void setFollowPosition(TA_Point *newFollowPosition);
    void setLockPosition(TA_Point newLockPosition);
    bool isLocked() {return locked && lockedX && lockedY;}
    void unlock() {locked = lockedX = lockedY = false;}
    void setBorder(TA_Rect newBorder) {border = newBorder;}
    void shake(double time) {shakeTime = time;}
    TA_Point getPosition() {return position;}
    TA_Point getRelative(TA_Point realPosition) {return realPosition - (position + shakeDelta);}
};


#endif // TA_CAMERA_H
