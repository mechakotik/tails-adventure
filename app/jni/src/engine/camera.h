#ifndef TA_CAMERA_H
#define TA_CAMERA_H

#include "geometry.h"

class TA_Camera {
private:
     TA_Point position, lockPosition;
     TA_Point *followPosition;
     TA_Rect border;
     const double groundSpeed = 1, airSpeed = 3, springSpeed = 5;
     bool locked = false, lockedX = false, lockedY = false;

public:
    void update(bool ground, bool spring);
    void setFollowPosition(TA_Point *newFollowPosition);
    void setLockPosition(TA_Point newLockPosition);
    bool isLocked() {return locked && lockedX && lockedY;}
    void unlock() {locked = lockedX = lockedY = false;}
    void setBorder(TA_Rect newBorder) {border = newBorder;}
    TA_Point getPosition() {return position;}
    TA_Point getRelative(TA_Point realPosition) {return realPosition - position;}
};


#endif // TA_CAMERA_H
