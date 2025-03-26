#include "moving_platform.h"
#include "character.h"

void TA_MovingPlatform::load(TA_Point startPosition, TA_Point endPosition, bool idle) {
    this->startPosition = startPosition;
    this->endPosition = endPosition;
    this->idle = idle;
    position = prevPosition = startPosition;

    TA_Sprite::load("maps/pm/platform.png");
    hitbox.setRectangle(TA_Point(0, 0), TA_Point(32, 16));
    updatePosition();
}

bool TA_MovingPlatform::update() {
    if(idle) {
        updateIdle();
    } else {
        movePlatform();
        moveCharacter();
    }
    updatePosition();
    return true;
}

void TA_MovingPlatform::updateIdle() {
    if(characterIsOnTop()) {
        idle = false;
    }
}

void TA_MovingPlatform::movePlatform() {
    prevPosition = position;
    if(moveByX()) {
        position.x += TA::sign(endPosition.x - startPosition.x) * (reverse ? -1.0F : 1.0F) * speed * TA::elapsedTime;
        if(exceedsBorder()) {
            position.x = (reverse ? startPosition.x : endPosition.x);
            reverse = !reverse;
        }
    } else {
        position.y += TA::sign(endPosition.y - startPosition.y) * (reverse ? -1.0F : 1.0F) * speed * TA::elapsedTime;
        if(exceedsBorder()) {
            position.y = (reverse ? startPosition.y : endPosition.y);
            reverse = !reverse;
        }
    }
}

void TA_MovingPlatform::moveCharacter() {
    if(characterIsOnTop()) {
        objectSet->getLinks().character->addToPosition(position - prevPosition);
    }
}

bool TA_MovingPlatform::characterIsOnTop() {
    TA_Polygon topHitbox;
    topHitbox.setRectangle(TA_Point(0, -0.05), TA_Point(32, 0.05));
    topHitbox.setPosition(prevPosition);

    if((objectSet->checkCollision(topHitbox) & TA_COLLISION_CHARACTER) != 0 &&
        objectSet->getLinks().character->isOnGround() && !objectSet->getLinks().character->isClimbing()) {
        return true;
    }
    return false;
}

bool TA_MovingPlatform::moveByX() {
    return TA::equal(startPosition.y, endPosition.y);
}

bool TA_MovingPlatform::exceedsBorder() {
    if(moveByX()) {
        float startX = startPosition.x, endX = endPosition.x;
        return !(std::min(startX, endX) <= position.x && position.x <= std::max(startX, endX));
    }
    float startY = startPosition.y, endY = endPosition.y;
    return !(std::min(startY, endY) <= position.y && position.y <= std::max(startY, endY));
}

int TA_MovingPlatform::getCollisionType() {
    if(idle) {
        return TA_COLLISION_SOLID;
    }
    return TA_COLLISION_MOVING_PLATFORM;
}
