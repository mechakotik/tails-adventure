#include <algorithm>
#include "camera.h"
#include "globals.h"

void TA_Camera::setFollowPosition(TA_Point *newFollowPosition)
{
    followPosition = newFollowPosition;
    position = *followPosition;
}

void TA_Camera::update(bool ground)
{
    double movementSpeed = airSpeed;
    if(ground) {
        movementSpeed = groundSpeed;
    }

    auto move = [&] (double current, double need) {
        if(current < need) {
            current = std::min(need, current + movementSpeed * gElapsedTime);
        }
        else {
            current = std::max(need, current - movementSpeed * gElapsedTime);
        }
        return current;
    };

    position.x = move(position.x, followPosition->x);
    position.y = move(position.y, followPosition->y);

    auto normalize = [&] (double current, double left, double right) {
        current = std::max(current, left);
        current = std::min(current, right);
        return current;
    };

    position.x = normalize(position.x, border.topLeft.x, border.bottomRight.x);
    position.y = normalize(position.y, border.topLeft.y, border.bottomRight.y);
}
