#include <algorithm>
#include "character.h"
#include "engine/globals.h"
#include "engine/tools.h"
#include "engine/error.h"
#include "engine/object_set.h"

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
    if(climb) {
        return;
    }

    if(!equal(velocity.x, 0)) {
        setFlip(velocity.x < 0);
    }

    updateTool();
    updateAnimation();
    updateFollowPosition();
}

bool TA_Character::checkCollision(TA_Polygon hitbox)
{
    int flags = 0;
    links.tilemap->checkCollision(hitbox, 1, flags);
    if((flags & TA_TILE_SOLID) != 0) {
        return true;
    }
    if((flags & TA_TILE_DAMAGE) != 0) {
        return true;
    }
    if(useHalfSolidTiles && (flags & TA_TILE_HALF_SOLID) != 0) {
        return true;
    }
    return false;
}

void TA_Character::updateCollisions()
{
    TA_Point topLeft, bottomRight;
    if(jump && velocity.y < 0) {
        topLeft = TA_Point(16, 9);
        bottomRight = TA_Point(32, 39);
    }
    else {
        topLeft = TA_Point(16, 12);
        bottomRight = TA_Point(32, 39);
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
            if(checkCollision(hitbox)) {
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

        bool collision = checkCollision(hitbox);
        hitbox.setPosition(climbPosition + TA_Point(0, velocity.y));
        bool collisionMoved = checkCollision(hitbox);
        if(collision != collisionMoved) {
            double left = 0, right = 1;
            while (right - left > gEpsilon) {
                double mid = (left + right) / 2;
                hitbox.setPosition(climbPosition + TA_Point(0, velocity.y * mid));
                if (checkCollision(hitbox) == collision) {
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

void TA_Character::updateTool()
{
    if(!controller.isJustPressed(TA_BUTTON_B)) {
        return;
    }
    switch(currentTool) {
        case TA_TOOL_BOMB:
            links.objectSet->spawnExplosion(position);
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
    followPosition = sourcePosition + TA_Point(22 - gScreenWidth / 2, 26 - gScreenHeight / 2);
}
