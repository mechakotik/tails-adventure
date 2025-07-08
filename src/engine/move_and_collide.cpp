#include <cassert>
#include "geometry.h"
#include "object_set.h"

std::pair<TA_Point, int> TA_ObjectSet::moveAndCollide(
    TA_Point position, TA_Point topLeft, TA_Point bottomRight, TA_Point velocity, int solidFlags, bool ground) {
    this->position = position;
    this->topLeft = topLeft;
    this->bottomRight = bottomRight;
    this->velocity = velocity;
    this->solidFlags = solidFlags;
    this->ground = ground;
    delta = {0, 0};

    if(!isGoodPosition(position + delta)) [[unlikely]] {
        popOut(4);
        if(!isGoodPosition(position + delta)) [[unlikely]] {
            popOut(32);
            if(!isGoodPosition(position + delta)) [[unlikely]] {
                return {{0, 0}, TA_COLLISION_ERROR};
            }
        }
    }

    moveByX();
    moveByY();
    return {delta, getCollisionFlags(position + delta, topLeft, bottomRight, solidFlags)};
}

void TA_ObjectSet::moveByX() {
    TA_Rect hitbox;
    if(ground) {
        hitbox.setRectangle(topLeft + TA_Point(0, 1), bottomRight - TA_Point(0, 1));
    } else {
        hitbox.setRectangle(topLeft, bottomRight);
    }

    auto isOutside = [&](float factor) {
        hitbox.setPosition(position + (delta + TA_Point(velocity.x * factor, 0)));
        return (checkCollision(hitbox) & solidFlags) == 0;
    };

    float left = 0, right = 1, eps = 0.001;
    if(!isOutside(eps * 2)) {
        left = right = 0;
    } else if(isOutside(1)) {
        left = right = 1;
    } else {
        while((right - left) * std::abs(velocity.x) > eps) {
            float mid = (left + right) / 2;
            if(isOutside(mid)) {
                left = mid;
            } else {
                right = mid;
            }
        }
    }

    delta.x += velocity.x * left;
}

void TA_ObjectSet::moveByY() {
    if(ground && isGoodPosition(position + delta + TA_Point(0, 2))) {
        ground = false;
    }
    if(ground) {
        delta.y -= 2;
        velocity.y = 4;
    }

    TA_Rect hitbox;
    hitbox.setRectangle(topLeft + TA_Point(0.005, 0), bottomRight - TA_Point(0.005, 0));

    auto isOutside = [&](float factor) {
        hitbox.setPosition(position + (delta + TA_Point(0, velocity.y * factor)));
        return (checkCollision(hitbox) & solidFlags) == 0;
    };

    float left = 0, right = 1, eps = 0.001;
    if(!isOutside(eps * 2)) {
        left = right = 0;
    } else if(isOutside(1)) {
        left = right = 1;
    }
    while((right - left) * std::abs(velocity.y) > eps) {
        float mid = (left + right) / 2;
        if(isOutside(mid)) {
            left = mid;
        } else {
            right = mid;
        }
    }

    delta.y += velocity.y * left;
}

void TA_ObjectSet::popOut(float area) {
    std::vector<std::pair<float, TA_Point>> directions;
    for(TA_Point add : {TA_Point(-area, 0), TA_Point(area, 0), TA_Point(0, -area), TA_Point(0, area)}) {
        directions.emplace_back(getFirstGood(add), add);
    }

    int min = 0;
    for(int pos = 1; pos < static_cast<int>(directions.size()); pos++) {
        if(directions[pos].first < directions[min].first) {
            min = pos;
        }
    }

    TA_Point add = directions[min].second * directions[min].first;
    if(isGoodPosition(position + (delta + add))) {
        delta += add;
    }
}

float TA_ObjectSet::getFirstGood(TA_Point add) {
    float left = 0, right = 1, eps = 0.001;
    while((right - left) * add.length() > eps) {
        float mid = (left + right) / 2;
        if(isGoodPosition(position + (delta + add * mid))) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return right;
}

bool TA_ObjectSet::isGoodPosition(TA_Point position) {
    TA_Rect hitbox;
    hitbox.setRectangle(topLeft, bottomRight);
    hitbox.setPosition(position);
    return (checkCollision(hitbox) & solidFlags) == 0;
}

int TA_ObjectSet::getCollisionFlags(TA_Point position, TA_Point topLeft, TA_Point bottomRight, int solidFlags) {
    TA_Rect hitbox;
    int flags = 0;
    hitbox.setRectangle(
        TA_Point(topLeft.x + 0.005, bottomRight.y), TA_Point(bottomRight.x - 0.005, bottomRight.y + 0.005));
    hitbox.setPosition(position);
    if((checkCollision(hitbox) & solidFlags) != 0) {
        flags |= TA_GROUND_COLLISION;
    }

    hitbox.setRectangle(TA_Point(topLeft.x + 0.005, topLeft.y - 0.005), TA_Point(bottomRight.x - 0.005, topLeft.y));
    if((checkCollision(hitbox) & solidFlags) != 0) {
        flags |= TA_CEIL_COLLISION;
    }

    hitbox.setRectangle(topLeft + TA_Point(-0.005, 1), bottomRight + TA_Point(0.005, -1));
    if((checkCollision(hitbox) & solidFlags) != 0) {
        flags |= TA_WALL_COLLISION;
    }

    return flags;
}
