#include <cassert>
#include "pawn.h"
#include "tools.h"
#include "error.h"

int TA_Pawn::moveAndCollide(TA_Point topLeft, TA_Point bottomRight, TA_Point velocity, bool ground)
{
    this->topLeft = topLeft;
    this->bottomRight = bottomRight;
    this->velocity = velocity;
    this->ground = ground;

    if(!isGoodPosition(position)) {
        popOut();
    }
    if(!isGoodPosition(position)) {
        return TA_COLLISION_ERROR;
    }

    moveByX();
    moveByY();
    return getCollisionFlags(topLeft, bottomRight);
}

void TA_Pawn::moveByX()
{
    TA_Polygon hitbox;
    if(ground) {
        hitbox.setRectangle(topLeft + TA_Point(0, 1), bottomRight - TA_Point(0, 1));
    }
    else {
        hitbox.setRectangle(topLeft, bottomRight);
    }

    double left = 0, right = 1, eps = 1e-5;
    while((right - left) * std::abs(velocity.x) > eps) {
        double mid = (left + right) / 2;
        hitbox.setPosition({position.x + mid * velocity.x, position.y});

        if(checkPawnCollision(hitbox)) {
            right = mid;
        }
        else {
            left = mid;
        }
    }

    position.x += velocity.x * left;
}

void TA_Pawn::moveByY()
{
    if(ground && isGoodPosition(position + TA_Point(0, 2))) {
        ground = false;
    }
    if(ground) {
        position.y -= 2;
        velocity.y = 4;
    }

    TA_Polygon hitbox;
    hitbox.setRectangle(topLeft, bottomRight);

    double left = 0, right = 1, eps = 1e-5;
    while((right - left) * std::abs(velocity.y) > eps) {
        double mid = (left + right) / 2;
        hitbox.setPosition({position.x, position.y + velocity.y * mid});

        if(checkPawnCollision(hitbox)) {
            right = mid;
        }
        else {
            left = mid;
        }
    }

    position.y += velocity.y * left;
}

void TA_Pawn::popOut()
{
    std::vector<std::pair<double, TA_Point>> directions;
    
    for(TA_Point delta : {TA_Point(-32, 0), TA_Point(32, 0), TA_Point(0, -32), TA_Point(0, 32)}) {
        directions.push_back({getFirstGood(delta), delta});
    }

    int min = 0;
    for(int pos = 1; pos < (int)directions.size(); pos ++) {
        if(directions[pos].first < directions[min].first) {
            min = pos;
        }
    }

    TA_Point add = directions[min].second * directions[min].first;
    if(isGoodPosition(position + add)) {
        position = position + add;
    }
}

double TA_Pawn::getFirstGood(TA_Point delta)
{
    double left = 0, right = 1, eps = 1e-5;
    while((right - left) * delta.length() > eps) {
        double mid = (left + right) / 2;
        if(isGoodPosition(position + delta * mid)) {
            right = mid;
        }
        else {
            left = mid;
        }
    }
    return right;
}

bool TA_Pawn::isGoodPosition(TA_Point position)
{
    TA_Polygon hitbox;
    hitbox.setRectangle(topLeft, bottomRight);
    hitbox.setPosition(position);
    return !checkPawnCollision(hitbox);
}

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
