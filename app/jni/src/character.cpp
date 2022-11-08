#include <algorithm>
#include "character.h"
#include "globals.h"
#include "tools.h"
#include "error.h"

void TA_Character::load(TA_GameScreenLinks newLinks)
{
    links = newLinks;
    position = {0, 128};
    updateFollowPosition();
    links.camera->setFollowPosition(&followPosition);
    controller.load();

    TA_Pawn::load("tails/tails.png", 48, 48);
    loadAnimationsFromFile("tails/animations.xml");
    setCamera(links.camera);
}

void TA_Character::update()
{
    controller.update();
    verticalMove();

    if(ground) {
        jump = false;
        if(controller.isJustPressed(TA_BUTTON_A)) {
            velocity.y = jmp;
            jump = true;
            jumpTime = 0;
            jumpReleased = false;
            ground = false;
        }
    }
    else {
        velocity.y += grv * gElapsedTime;
        velocity.y = std::min(velocity.y, topY * gElapsedTime);
        if(jump && !jumpReleased) {
            jumpTime += gElapsedTime;
            if(controller.isPressed(TA_BUTTON_A) && jumpTime < maxJumpTime) {
                velocity.y = jmp;
            }
            else if(!controller.isPressed(TA_BUTTON_A)) {
                jumpReleased = true;
            }
        }
    }

    updateCollisions();
    setPosition(position);

    if(!equal(velocity.x, 0)) {
        setFlip(velocity.x < 0);
    }

    updateAnimation();
    updateFollowPosition();
}

bool TA_Character::checkCollision(TA_Polygon hitbox)
{
    int flags = 0;
    return links.tilemap->checkCollision(hitbox, 1, flags);
}

void TA_Character::updateCollisions()
{
    TA_Point baseTopLeft, baseBottomRight;
    if(jump && velocity.y < 0) {
        baseTopLeft = TA_Point(15, 9);
        baseBottomRight = TA_Point(33, 39);
    }
    else {
        baseTopLeft = TA_Point(15, 12);
        baseBottomRight = TA_Point(33, 39);
    }

    TA_Polygon xHitbox, yHitbox;
    xHitbox.setRectangle(baseTopLeft + TA_Point(0, 1), baseBottomRight - TA_Point(0, 1));
    yHitbox.setRectangle(baseTopLeft + TA_Point(1, 0), baseBottomRight - TA_Point(1, 0));
    moveAndCollide(xHitbox, yHitbox, velocity, ground);

    TA_Polygon hitbox;
    hitbox.setRectangle(TA_Point(baseTopLeft.x + 1, baseBottomRight.y), TA_Point(baseBottomRight.x - 1, baseBottomRight.y + 0.001));
    hitbox.setPosition(position);
    if(checkCollision(hitbox)) {
        ground = true;
        velocity.y = 0;
    }
    else {
        ground = false;
    }

    hitbox.setRectangle(TA_Point(baseTopLeft.x + 1, baseTopLeft.y - 0.001), TA_Point(baseBottomRight.x - 1, baseTopLeft.y));
    if(checkCollision(hitbox)) {
        velocity.y = std::max(velocity.y, double(0));
    }

    hitbox.setRectangle(baseTopLeft + TA_Point(-0.001, 1), baseBottomRight + TA_Point(0.001, -1));
    hitbox.setPosition(position);
    wall = checkCollision(hitbox);

    position.x = std::max(position.x, double(0));
    position.x = std::min(position.x, double(links.tilemap->getWidth()));
    position.y = std::max(position.y, double(0));
    position.y = std::min(position.y, double(links.tilemap->getHeight()));
}

void TA_Character::updateAnimation()
{
    if(ground) {
        if(equal(velocity.x, 0)) {
            setAnimation("idle");
        }
        else if(wall) {
            setAnimation("push");
        }
        else {
            setAnimation("walk");
        }
    }
    else {
        if(velocity.y < 0) {
            setAnimation("jump_up");
        }
        else {
            setAnimation("jump_down");
        }
    }
}

void TA_Character::verticalMove()
{
    TA_Direction direction = controller.getDirection();

    if(direction == TA_DIRECTION_RIGHT) {
        velocity.x += acc * gElapsedTime;
        velocity.x = std::min(velocity.x, topX * gElapsedTime);
    }
    else if(direction == TA_DIRECTION_LEFT) {
        velocity.x -= acc * gElapsedTime;
        velocity.x = std::max(velocity.x, -topX * gElapsedTime);
    }
    else {
        if(velocity.x > 0) {
            velocity.x = std::max(double(0), velocity.x - acc * gElapsedTime);
        }
        else {
            velocity.x = std::min(double(0), velocity.x + acc * gElapsedTime);
        }
    }
}

void TA_Character::updateFollowPosition()
{
    followPosition = position + TA_Point(22 - gScreenWidth / 2, 26 - gScreenHeight / 2);
}
