#include <cassert>
#include "pawn.h"
#include "tools.h"
#include "error.h"

int TA_Pawn::getCollisionFlags(TA_Point topLeft, TA_Point bottomRight)
{
    TA_Polygon hitbox;
    int flags = 0;
    hitbox.setRectangle(TA_Point(topLeft.x + 0.001, bottomRight.y), TA_Point(bottomRight.x - 0.001, bottomRight.y + 0.001));
    hitbox.setPosition(position);
    if(checkPawnCollision(hitbox)) {
        flags |= TA_GROUND_COLLISION;
    }

    hitbox.setRectangle(TA_Point(topLeft.x + 0.001, topLeft.y - 0.001), TA_Point(bottomRight.x - 0.001, topLeft.y));
    if(checkPawnCollision(hitbox)) {
        flags |= TA_CEIL_COLLISION;
    }

    hitbox.setRectangle(topLeft + TA_Point(-0.001, 1), bottomRight + TA_Point(0.001, -1));
    if(checkPawnCollision(hitbox)) {
        flags |= TA_WALL_COLLISION;
    }

    return flags;
}

int TA_Pawn::moveAndCollide(TA_Point topLeft, TA_Point bottomRight, TA_Point velocity, bool ground)
{
    TA_Polygon xHitbox, yHitbox;
    if(ground) {
        xHitbox.setRectangle(topLeft + TA_Point(0, 1), bottomRight - TA_Point(0, 1));
    }
    else {
        xHitbox.setRectangle(topLeft, bottomRight);
    }
    yHitbox.setRectangle(topLeft, bottomRight);

    TA_Point endPosition = position + velocity;
    double left = 0, right = 1; {
        auto shiftRight = [&](double mid) {
            xHitbox.setPosition(position + (endPosition - position) * TA_Point(mid, 0));
            return checkPawnCollision(xHitbox);
        };
        while((right - left) * std::abs(endPosition.x - position.x) > TA::epsilon) {
            double mid = (left + right) / 2;
            if(shiftRight(mid)) {
                right = mid;
            }
            else {
                left = mid;
            }
        }
    }
    position.x += (endPosition.x - position.x) * left;
    yHitbox.setPosition(position + TA_Point(0, 2));
    if(!checkPawnCollision(yHitbox)) {
        ground = false;
    }
    if(ground) {
        position.y -= 2;
        velocity.y = 4;
        endPosition = position + velocity;
    }
    left = 0, right = 1; {
        auto shiftRight = [&](double mid) {
            yHitbox.setPosition(position + (endPosition - position) * TA_Point(0, mid));
            return checkPawnCollision(yHitbox);
        };
        while ((right - left) * std::abs(endPosition.y - position.y) > TA::epsilon) {
            double mid = (left + right) / 2;
            if (shiftRight(mid)) {
                right = mid;
            }
            else {
                left = mid;
            }
        }
    }
    position.y += (endPosition.y - position.y) * left;
    yHitbox.setPosition(position);
    int flags = 0;

    if(checkPawnCollision(yHitbox)) {
        flags |= TA_COLLISION_ERROR;
        double delta = 0;

        auto good = [&] (TA_Point newPosition) {
            yHitbox.setPosition(newPosition);
            if(checkPawnCollision(yHitbox)) {
                return false;
            }
            return true;
        };

        while(true) {
            delta += 0.05;
            if(good(position - TA_Point(delta, 0))) {
                position.x -= delta;
                break;
            }
            if(good(position + TA_Point(delta, 0))) {
                position.x += delta;
                break;
            }
            if(good(position - TA_Point(0, delta))) {
                position.y -= delta;
                break;
            }
            if(good(position + TA_Point(0, delta))) {
                position.y += delta;
                break;
            }
        }
    }

    flags |= getCollisionFlags(topLeft, bottomRight);
    return flags;
}
