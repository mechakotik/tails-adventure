#include "sea_fox.h"
#include "controller.h"
#include "object_set.h"

void TA_SeaFox::load(TA_Links links)
{
    this->links = links;
    links.camera->setFollowPosition(&followPosition);

    TA_Sprite::load("tails/seafox.png", 32, 32);
    TA_Sprite::loadAnimationsFromFile("tails/seafox.xml");
    TA_Sprite::setCamera(links.camera);
    TA_Sprite::setAnimation("idle");
}

void TA_SeaFox::update()
{
    physicsStep();

    updateDirection();
    updateFollowPosition();
    updateDrill();

    setFlip(flip);
}

void TA_SeaFox::physicsStep()
{
    auto updateSpeed = [] (double &currentSpeed, double neededSpeed, double drag)
    {
        if(currentSpeed > neededSpeed) {
            currentSpeed = std::max(neededSpeed, currentSpeed - drag * TA::elapsedTime);
        }
        else {
            currentSpeed = std::min(neededSpeed, currentSpeed + drag * TA::elapsedTime);
        }
    };

    if(links.controller->getDirection() != TA_DIRECTION_MAX) {
        TA_Point vector = links.controller->getDirectionVector();
        updateSpeed(velocity.x, vector.x, inputDrag);
        updateSpeed(velocity.y, vector.y, inputDrag);
    }
    else {
        updateSpeed(velocity.x, 0, horizontalDrag);
        updateSpeed(velocity.y, 0, verticalDrag);
    }

    moveAndCollide(TA_Point(8, 4), TA_Point(24, 30), velocity * TA::elapsedTime, false);
    TA_Sprite::setPosition(position);
}

bool TA_SeaFox::checkPawnCollision(TA_Polygon &hitbox)
{
    int flags = links.objectSet->checkCollision(hitbox);
    if(flags & TA_COLLISION_SOLID) {
        return true;
    }
    return false;
}

void TA_SeaFox::updateDirection()
{
    if(flip != neededFlip) {
        if(!isAnimated()) {
            setAnimation("idle");
            flip = neededFlip;
        }
    }
    else if(links.controller->isJustPressed(TA_BUTTON_A)) {
        setAnimation("rotate");
        neededFlip = !flip;
    }
}

void TA_SeaFox::updateFollowPosition()
{
    followPosition = position + TA_Point(getWidth() / 2, getHeight() / 2) - TA_Point(TA::screenWidth / 2, TA::screenHeight / 2);
    followPosition.x += (flip ? -1 : 1) * (TA::screenWidth * 0.15);
}

void TA_SeaFox::updateDrill()
{
    if(flip) {
        drillHitbox.setRectangle(TA_Point(2, 10), TA_Point(16, 24));
    }
    else {
        drillHitbox.setRectangle(TA_Point(24, 10), TA_Point(30, 24));
    }
    drillHitbox.setPosition(position);
}
