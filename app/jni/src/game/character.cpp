#include <algorithm>
#include "character.h"
#include "engine/tools.h"
#include "engine/error.h"
#include "engine/object_set.h"
#include "engine/gamepad.h"

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
    controller.setAnalogStick(helitail);
    controller.update();

    if(climb) {
        if(!isAnimated()) {
            position = climbPosition;
            climb = false;
        }
        else if(climbHigh && getAnimationFrame() == 1) {
            setPosition(position.x, position.y - 8);
        }
        else if(getAnimationFrame() == 2) {
            setPosition(position.x, climbPosition.y);
        }
        updateFollowPosition();
        return;
    }

    if(throwing) {
        if(!isAnimated()) {
            throwing = false;
        }
        updateFollowPosition();
        return;
    }

    if(ground) {
        updateGround();
    }
    else if(helitail) {
        updateHelitail();
    }
    else {
        updateAir();
    }

    updateCollisions();
    setPosition(position);
    if(climb) {
        return;
    }
    setFlip(flip);
    updateTool();
    if(throwing) {
        return;
    }
    updateAnimation();
    updateFollowPosition();
}

void TA_Character::updateGround()
{
    verticalMove();
    jump = false;
    if(controller.isJustPressed(TA_BUTTON_A)) {
        velocity.y = jmp;
        jump = true;
        jumpTime = 0;
        jumpReleased = false;
        ground = false;
    }
}

void TA_Character::updateAir()
{
    verticalMove();
    velocity.y += grv * TA::elapsedTime;
    velocity.y = std::min(velocity.y, topY);
    if(jump && !jumpReleased) {
        jumpTime += TA::elapsedTime;
        if(controller.isPressed(TA_BUTTON_A) && jumpTime < maxJumpTime) {
            velocity.y = jmp;
        }
        else if(!controller.isPressed(TA_BUTTON_A)) {
            jumpReleased = true;
        }
    }
    if(jump && jumpReleased && controller.isJustPressed(TA_BUTTON_A)) {
        jump = false;
        helitail = true;
        helitailTime = 0;
    }
}

void TA_Character::updateHelitail()
{
    auto process = [&](double &x, double need) {
        if(x > need) {
            x = std::max(need, x - helitailAcc);
        }
        else {
            x = std::min(need, x + helitailAcc);
        }
    };

    helitailTime += TA::elapsedTime;
    TA_Point vector;
    TA_Direction direction = controller.getDirection();
    if(direction != TA_DIRECTION_MAX) {
        vector = controller.getDirectionVector();
    }
    if(helitailTime > maxHelitailTime) {
        vector.y = 1;
    }

    if(direction == TA_DIRECTION_LEFT) {
        flip = true;
    }
    else if(direction == TA_DIRECTION_RIGHT) {
        flip = false;
    }

    process(velocity.x, vector.x * helitailTop);
    process(velocity.y, vector.y * helitailTop);
}

void TA_Character::verticalMove()
{
    TA_Direction direction = controller.getDirection();

    if(direction == TA_DIRECTION_RIGHT) {
        flip = false;
        velocity.x += acc * TA::elapsedTime;
        velocity.x = std::min(velocity.x, topX);
    }
    else if(direction == TA_DIRECTION_LEFT) {
        flip = true;
        velocity.x -= acc * TA::elapsedTime;
        velocity.x = std::max(velocity.x, -topX);
    }
    else {
        if(velocity.x > 0) {
            velocity.x = std::max(double(0), velocity.x - acc * TA::elapsedTime);
        }
        else {
            velocity.x = std::min(double(0), velocity.x + acc * TA::elapsedTime);
        }
    }
}

bool TA_Character::checkPawnCollision(TA_Polygon hitbox)
{
    int flags = 0;
    links.objectSet->checkCollision(hitbox, flags);
    if((flags & TA_COLLISION_SOLID) != 0) {
        return true;
    }
    if((flags & TA_COLLISION_DAMAGE) != 0) {
        return true;
    }
    if(useHalfSolidTiles && (flags & TA_COLLISION_HALF_SOLID) != 0) {
        return true;
    }
    return false;
}

void TA_Character::updateCollisions()
{
    TA_Point topLeft, bottomRight;
    if(jump && velocity.y < 0) {
        topLeft = TA_Point(18, 9);
        bottomRight = TA_Point(30, 39);
    }
    else {
        topLeft = TA_Point(18, 12);
        bottomRight = TA_Point(30, 39);
    }

    if(ground) {
        useHalfSolidTiles = true;
    }
    else if(velocity.y > 0.01) {
        if(!useHalfSolidTiles) {
            useHalfSolidTiles = true;
            TA_Polygon hitbox;
            hitbox.setRectangle(topLeft, bottomRight);
            hitbox.setPosition(position);
            if(checkPawnCollision(hitbox)) {
                useHalfSolidTiles = false;
            }
        }
    }
    else {
        useHalfSolidTiles = false;
    }

    updateClimb();
    if(climb) {
        return;
    }
    int flags = moveAndCollide(topLeft, bottomRight, velocity, ground);
    if(flags & TA_GROUND_COLLISION) {
        ground = true;
        helitail = false;
        velocity.y = 0;
    }
    else {
        ground = false;
    }
    wall = bool(flags & TA_WALL_COLLISION);
    if(flags & TA_CEIL_COLLISION) {
        velocity.y = std::max(velocity.y, double(0));
        if(jump) {
            jumpReleased = true;
        }
    }
}

void TA_Character::updateClimb()
{
    if(!wall) {
        return;
    }

    auto updateClimbPosition = [&](int height) {
        if(climb) {
            return;
        }
        climbPosition = position;
        if(controller.getDirection() == TA_DIRECTION_LEFT) {
            climbPosition = climbPosition + TA_Point(-13, -height - (ground ? 0.025 : 0));
            setFlip(true);
        }
        else if(controller.getDirection() == TA_DIRECTION_RIGHT) {
            climbPosition = climbPosition + TA_Point(13, -height - (ground ? 0.025 : 0));
            setFlip(false);
        }
        else {
            return;
        }

        TA_Point topLeft = TA_Point(16, 12), bottomRight = TA_Point(32, 39);
        TA_Polygon hitbox;
        hitbox.setRectangle(topLeft, bottomRight);
        hitbox.setPosition(climbPosition);
        if(ground) {
            velocity.y = 0.05;
        }

        bool collision = checkPawnCollision(hitbox);
        hitbox.setPosition(climbPosition + TA_Point(0, velocity.y));
        bool collisionMoved = checkPawnCollision(hitbox);
        if(collision != collisionMoved) {
            double left = 0, right = 1;
            while (right - left > TA::epsilon) {
                double mid = (left + right) / 2;
                hitbox.setPosition(climbPosition + TA_Point(0, velocity.y * mid));
                if (checkPawnCollision(hitbox) == collision) {
                    left = mid;
                }
                else {
                    right = mid;
                }
            }
            climbPosition.y += velocity.y * left;
            TA_Point delta = climbPosition - position + TA_Point(0, 0.01);
            int flags = getCollisionFlags(topLeft + delta, bottomRight + delta);
            if(flags == TA_GROUND_COLLISION) {
                ground = true;
                jump = wall = false;
                climb = true;
                climbTime = 0;
                if(height == 32) {
                    setAnimation("climb_high");
                    climbHigh = true;
                }
                else {
                    setAnimation("climb");
                    climbHigh = false;
                }
                if(controller.getDirection() == TA_DIRECTION_LEFT) {
                    position = climbPosition + TA_Point(13, height);
                }
                else {
                    position = climbPosition + TA_Point(-13, height);
                }
            }
        }
        if(ground) {
            velocity.y = 0;
        }
    };

    updateClimbPosition(32);
    updateClimbPosition(16);
}

void TA_Character::updateAnimation()
{
    if(ground) {
        if(TA::equal(velocity.x, 0)) {
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
        if(helitail) {
            if(helitailTime > maxHelitailTime) {
                setAnimation("helitail_tired");
            }
            else {
                setAnimation("helitail");
            }
        }
        else if(velocity.y < 0) {
            setAnimation("jump_up");
        }
        else {
            setAnimation("jump_down");
        }
    }
}

void TA_Character::updateTool()
{
    if(!controller.isJustPressed(TA_BUTTON_B)) {
        return;
    }
    switch(currentTool) {
        case TA_TOOL_BOMB:
            links.objectSet->spawnBomb(position + TA_Point((flip? 27 : 8), 16), flip);
            setAnimation("throw");
            throwing = true;
            break;

        default:
            break;
    }
}

void TA_Character::updateFollowPosition()
{
    TA_Point sourcePosition = position;
    if(climb) {
        sourcePosition.x = climbPosition.x;
    }
    followPosition = sourcePosition + TA_Point(22 - TA::screenWidth / 2, 26 - TA::screenHeight / 2);
}
