#include "character.h"
#include "object_set.h"
#include "error.h"
#include "save.h"

bool TA_Character::checkPawnCollision(TA_Polygon &checkHitbox)
{
    int flags = 0;
    links.objectSet->checkCollision(checkHitbox, flags, checkHitbox.getPosition().y + 36);
    if((flags & TA_COLLISION_SOLID) || (flags & TA_COLLISION_PUSHABLE)) {
        return true;
    }
    if(useHalfSolidTiles && (flags & TA_COLLISION_HALF_SOLID)) {
        return true;
    }
    return false;
}

void TA_Character::updateCollisions()
{
    if(remoteRobot) {
        topLeft = TA_Point(18, 27);
    }
    else if(jump && velocity.y < 0) {
        topLeft = TA_Point(18, 9);
    }
    else if(crouch) {
        topLeft = TA_Point(18, 24);
    }
    else {
        topLeft = TA_Point(18, 12);
    }
    bottomRight = TA_Point(30, 39);
    hitbox.setRectangle(topLeft, bottomRight);

    if(ground) {
        useHalfSolidTiles = true;
    }
    else if(velocity.y > -0.01) {
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
    if(state == STATE_CLIMB_LOW || state == STATE_CLIMB_HIGH) {
        setPosition(position);
        return;
    }
    int flags = moveAndCollide(topLeft, bottomRight, velocity * TA::elapsedTime, ground);
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
        if(jump) {
            jumpSpeed = std::max(jumpSpeed, double(-0.3));
            jumpReleased = true;
        }
        else {
            velocity.y = std::max(velocity.y, double(-0.3));
        }
    }

    if(!remoteRobot && !hurt && invincibleTimeLeft <= 0) {
        auto handleDamage = [&] (TA_Polygon &hitbox, int sign) {
            if(hurt) {
                return;
            }
            links.objectSet->checkCollision(hitbox, flags);
            if((flags & TA_COLLISION_DAMAGE) == 0) {
                return;
            }
            hurt = true;
            ground = helitail = false;
            position.y -= 1;
            velocity.x = hurtXsp * sign;
            velocity.y = hurtYsp;
            addRings(-1);
        };
        TA_Polygon leftHalf, rightHalf; {
            double middleX = (topLeft.x + bottomRight.x) / 2;
            leftHalf.setRectangle(topLeft - TA_Point(0.1, 0.1), {middleX, bottomRight.y + 0.1});
            rightHalf.setRectangle({middleX, topLeft.y - 0.1}, bottomRight + TA_Point(0.1, 0.1));
            leftHalf.setPosition(position);
            rightHalf.setPosition(position);
        }
        if(flip) {
            handleDamage(rightHalf, -1);
            handleDamage(leftHalf, 1);
        }
        else {
            handleDamage(leftHalf, 1);
            handleDamage(rightHalf, -1);
        }
    }
    else if(hurt) {
        if(ground) {
            hurt = false;
            invincibleTimeLeft = invincibleTime;
            if(rings < 0) {
                setAnimation("death");
                state = STATE_DEAD;
            }
        }
    }
    else {
        invincibleTimeLeft -= TA::elapsedTime;
    }

    hitbox.setPosition(position);
}

void TA_Character::updateClimb()
{
    if(remoteRobot || !wall || !TA::equal(deltaX, 0)) {
        return;
    }

    auto updateClimbPosition = [&](int height) {
        if(state == STATE_CLIMB_LOW || state == STATE_CLIMB_HIGH) {
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
                jump = spring = wall = false;
                climbTime = 0;
                if(height == 32) {
                    setAnimation("climb_high");
                    state = STATE_CLIMB_HIGH;
                }
                else {
                    setAnimation("climb");
                    state = STATE_CLIMB_LOW;
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

void TA_Character::updateObjectCollision()
{
    TA_Polygon hitbox;
    hitbox.setPosition(position);
    hitbox.setRectangle(topLeft - TA_Point(0.05, 0.05), bottomRight + TA_Point(0.05, 0.05));

    int flags = links.objectSet->checkCollision(hitbox);
    if(flags & TA_COLLISION_SPRING) {
        jumpSound.play();
        jumpSpeed = springYsp;
        jump = spring = true;
        jumpReleased = true;
        ground = false;
    }
}
