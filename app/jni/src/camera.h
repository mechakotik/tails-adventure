#ifndef TA_CAMERA_H
#define TA_CAMERA_H

#include "geometry.h"

class TA_Camera {
private:
     TA_Point position;
     TA_Point *followPosition;
     TA_Rect border;
     const double movementSpeed = 2;

public:
    void update();
    void setFollowPosition(TA_Point *newFollowPosition);
    void setBorder(TA_Rect newBorder) {border = newBorder;}
    TA_Point getRelative(TA_Point realPosition) {return realPosition - position;}
};


#endif // TA_CAMERA_H
