#include "pawn.h"
#include "globals.h"

void TA_Pawn::moveAndCollide(TA_Polygon xHitbox, TA_Polygon yHitbox, TA_Point velocity, bool ground)
{
    TA_Point endPosition = position + velocity;
    double left = 0, right = 1;
    while(right - left > gEpsilon) {
        double mid = (left + right) / 2;
        xHitbox.setPosition(position + (endPosition - position) * TA_Point(mid, 0));
        if(checkCollision(xHitbox)) {
            right = mid;
        }
        else {
            left = mid;
        }
    }
    position.x += (endPosition.x - position.x) * left;

    yHitbox.setPosition(position + TA_Point(0, 8));
    if(!checkCollision(yHitbox)) {
        ground = false;
    }
    if(ground) {
        yHitbox.setPosition(position);
        if(checkCollision(yHitbox)) {
            velocity.y = -8;
        }
        else {
            velocity.y = 8;
        }
        endPosition = position + velocity;
    }
    left = 0, right = 1;
    while(right - left > gEpsilon) {
        double mid = (left + right) / 2;
        yHitbox.setPosition(position + (endPosition - position) * TA_Point(0, mid));
        if(checkCollision(yHitbox) != (ground && velocity.y < 0)) {
            right = mid;
        }
        else {
            left = mid;
        }
    }
    position.y += (endPosition.y - position.y) * left;
}
