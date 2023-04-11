#include "pawn.h"
#include "tools.h"
#include "error.h"

int TA_Pawn::getCollisionFlags(TA_Point topLeft, TA_Point bottomRight)
{
    TA_Polygon hitbox;
    int flags = 0;
    hitbox.setRectangle(TA_Point(topLeft.x + 1, bottomRight.y), TA_Point(bottomRight.x - 1, bottomRight.y + 0.001));
    hitbox.setPosition(position);
    if(checkPawnCollision(hitbox)) {
        flags |= TA_GROUND_COLLISION;
    }

    hitbox.setRectangle(TA_Point(topLeft.x + 1, topLeft.y - 0.001), TA_Point(bottomRight.x - 1, topLeft.y));
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
    velocity = velocity * TA_Point(TA::elapsedTime, TA::elapsedTime);
    TA_Polygon xHitbox, yHitbox;
    xHitbox.setRectangle(topLeft + TA_Point(0, 1), bottomRight - TA_Point(0, 1));
    yHitbox.setRectangle(topLeft + TA_Point(1, 0), bottomRight - TA_Point(1, 0));

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

    yHitbox.setPosition(position + TA_Point(0, 8));
    if(!checkPawnCollision(yHitbox)) {
        ground = false;
    }
    if(ground) {
        yHitbox.setPosition(position);
        if(checkPawnCollision(yHitbox)) {
            velocity.y = -8;
        }
        else {
            velocity.y = 8;
        }
        endPosition = position + velocity;
    }
    left = 0, right = 1; {
        auto shiftRight = [&](double mid) {
            yHitbox.setPosition(position + (endPosition - position) * TA_Point(0, mid));
            return checkPawnCollision(yHitbox) != (ground && velocity.y < 0);
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

    int flags = 0;

    xHitbox.setPosition(position);
    if(checkPawnCollision(xHitbox)) {
        flags |= TA_COLLISION_ERROR;
        double delta = 0;
        while(true) {
            delta += 0.1;
            xHitbox.setPosition(position + TA_Point(delta, 0));
            if(!checkPawnCollision(xHitbox)) {
                position.x += delta;
                break;
            }
            xHitbox.setPosition(position - TA_Point(delta, 0));
            if(!checkPawnCollision(xHitbox)) {
                position.x -= delta;
                break;
            }
        }
    }

    if(!ground) {
        yHitbox.setPosition(position);
        if(checkPawnCollision(yHitbox)) {
            flags |= TA_COLLISION_ERROR;
            double delta = 0;
            while(true) {
                delta += 0.1;
                yHitbox.setPosition(position + TA_Point(0, delta));
                if(!checkPawnCollision(yHitbox)) {
                    position.y += delta;
                    break;
                }
                xHitbox.setPosition(position - TA_Point(0, delta));
                if(!checkPawnCollision(xHitbox)) {
                    position.y -= delta;
                    break;
                }
            }
        }
    }

    flags |= getCollisionFlags(topLeft, bottomRight);
    return flags;
}
