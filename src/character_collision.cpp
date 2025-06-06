#include "character.h"
#include "error.h"
#include "object_set.h"
#include "ring.h"
#include "save.h"
#include "splash.h"
#include "tilemap.h"

bool TA_Character::checkPawnCollision(TA_Rect& checkHitbox) {
    int flags = 0;
    links.objectSet->checkCollision(checkHitbox, flags);
    if((flags & (TA_COLLISION_SOLID | TA_COLLISION_PUSHABLE)) != 0) {
        return true;
    }
    if(useSolidUpTiles && (flags & TA_COLLISION_SOLID_UP) != 0) {
        return true;
    }
    if(useSolidDownTiles && (flags & TA_COLLISION_SOLID_DOWN) != 0) {
        return true;
    }
    if(useMovingPlatforms && (flags & TA_COLLISION_MOVING_PLATFORM) != 0) {
        return true;
    }
    return false;
}

void TA_Character::updateCollisions() {
    if(remoteRobot) {
        topLeft = TA_Point(18, 27);
    } else if(hurt) {
        topLeft = TA_Point(18, 12);
    } else if(jump && velocity.y < 0) {
        topLeft = TA_Point(18, 9);
    } else if(crouch) {
        topLeft = TA_Point(18, 22);
    } else {
        topLeft = TA_Point(18, 12);
    }
    bottomRight = TA_Point(30, 39);

    hitbox.setRectangle(topLeft, bottomRight);

    if(ground) {
        useSolidUpTiles = true;
        useSolidDownTiles = false;
    } else {
        if(velocity.y > -0.01 && !useSolidUpTiles) {
            useSolidUpTiles = true;
            TA_Rect hitbox;
            hitbox.setRectangle(topLeft, bottomRight);
            hitbox.setPosition(position);
            if(checkPawnCollision(hitbox)) {
                useSolidUpTiles = false;
            }
        } else if(velocity.y <= -0.01) {
            useSolidUpTiles = false;
        }
        if(velocity.y < 0.01 && !useSolidDownTiles) {
            useSolidDownTiles = true;
            TA_Rect hitbox;
            hitbox.setRectangle(topLeft, bottomRight);
            hitbox.setPosition(position);
            if(checkPawnCollision(hitbox)) {
                useSolidDownTiles = false;
            }
        } else if(velocity.y >= 0.01) {
            useSolidDownTiles = false;
        }
    }

    updateClimb();
    if(state == STATE_CLIMB_LOW || state == STATE_CLIMB_HIGH) {
        setPosition(position);
        return;
    }

    useMovingPlatforms = true;
    TA_Point prevPosition = position;

    TA_Point positionDelta;
    if(TA::levelPath.substr(0, 7) == "maps/ci") {
        positionDelta = velocity * TA::elapsedTime;
    } else {
        positionDelta = (velocity + windVelocity) * TA::elapsedTime;
    }

    if(ground) {
        int flags = links.objectSet->checkCollision(hitbox);
        if(flags & TA_COLLISION_CONVEYOR_BELT_LEFT) {
            positionDelta.x -= 0.8 * TA::elapsedTime;
        }
        if(flags & TA_COLLISION_CONVEYOR_BELT_RIGHT) {
            positionDelta.x += 0.8 * TA::elapsedTime;
        }
    }

    int flags = moveAndCollide(topLeft, bottomRight, positionDelta, ground);

    if(flags & TA_COLLISION_ERROR) {
        position = prevPosition;
        useSolidUpTiles = useMovingPlatforms = false;
        flags = moveAndCollide(topLeft, bottomRight, positionDelta, ground);
    }

    if(flags & TA_GROUND_COLLISION) {
        ground = true;
        helitail = false;
        velocity.y = 0;
    } else {
        ground = false;
    }

    wall = bool(flags & TA_WALL_COLLISION);
    if(wall && hurt) {
        velocity.x = 0;
    }

    if(flags & TA_CEIL_COLLISION) {
        ceiling = true;
        if(jump) {
            jumpSpeed = std::max(jumpSpeed, float(-0.3));
            jumpReleased = true;
        } else {
            velocity.y = std::max(velocity.y, float(-0.3));
        }
    } else {
        ceiling = false;
    }

    if(!remoteRobot && !hurt && invincibleTimeLeft <= 0) {
        auto handleDamage = [&](TA_Rect& hitbox, int sign) {
            if(hurt) {
                return;
            }
            links.objectSet->checkCollision(hitbox, flags);
            if((flags & TA_COLLISION_DAMAGE) == 0) {
                return;
            }
            hurt = true;
            ground = helitail = jump = false;
            position.y -= 1;
            velocity.x = hurtXsp * sign;
            velocity.y = hurtYsp;
            damageSound.play();
            TA::gamepad::rumble(0.75, 0.75, 20);

            if(TA::save::getParameter("ring_drop")) {
                dropRings();
                links.objectSet->addRings(-4);
            } else {
                links.objectSet->addRings(-2);
            }
        };

        TA_Rect leftHalf, rightHalf;
        {
            float middleX = (topLeft.x + bottomRight.x) / 2;
            leftHalf.setRectangle(topLeft - TA_Point(0.01, 0.01), {middleX, bottomRight.y + 0.01});
            rightHalf.setRectangle({middleX, topLeft.y - 0.01}, bottomRight + TA_Point(0.01, 0.01));
            leftHalf.setPosition(position);
            rightHalf.setPosition(position);
        }
        if(flip) {
            handleDamage(rightHalf, -1);
            handleDamage(leftHalf, 1);
        } else {
            handleDamage(leftHalf, 1);
            handleDamage(rightHalf, -1);
        }
    } else if(hurt) {
        if(ground) {
            hurt = false;
            invincibleTimeLeft = invincibleTime;
            if(rings <= 0) {
                setAnimation("death");
                state = STATE_DEAD;
                TA::sound::playMusic("sound/death.vgm");
            }
        }
    }

    if(TA::levelPath.substr(0, 7) == "maps/ci" && (!TA::equal(windVelocity.x, 0) || !TA::equal(windVelocity.y, 0))) {
        ground = jump = jumpReleased = false;
    }

    hitbox.setPosition(position);
}

void TA_Character::dropRings() {
    if(rings <= 4) {
        return;
    }
    TA_Point ringPosition = position + TA_Point(20, 20);
    links.objectSet->spawnObject<TA_Ring>(ringPosition, TA_Point(1.5, -1), 64);
    links.objectSet->spawnObject<TA_Ring>(ringPosition, TA_Point(-1.5, -1), 64);
    links.objectSet->spawnObject<TA_Ring>(ringPosition, TA_Point(0.75, -2), 64);
    links.objectSet->spawnObject<TA_Ring>(ringPosition, TA_Point(-0.75, -2), 64);
}

void TA_Character::updateClimb() {
    if(remoteRobot || !wall || !TA::equal(deltaX, 0)) {
        return;
    }
    if(helitail && TA::levelPath == "maps/pm/pm4") {
        return;
    }

    auto updateClimbPosition = [&](int height) {
        if(state == STATE_CLIMB_LOW || state == STATE_CLIMB_HIGH) {
            return;
        }
        climbPosition = position;
        if(links.controller->getDirection() == TA_DIRECTION_LEFT) {
            climbPosition = climbPosition + TA_Point(-13, -height - (ground ? 0.025 : 0));
            setFlip(true);
        } else if(links.controller->getDirection() == TA_DIRECTION_RIGHT) {
            climbPosition = climbPosition + TA_Point(13, -height - (ground ? 0.025 : 0));
            setFlip(false);
        } else {
            return;
        }

        TA_Point topLeft = TA_Point(16, 12), bottomRight = TA_Point(32, 39);
        TA_Rect hitbox;
        hitbox.setRectangle(topLeft, bottomRight);
        hitbox.setPosition(climbPosition);
        if(ground) {
            velocity.y = 0.05;
        }

        bool collision = checkPawnCollision(hitbox);
        hitbox.setPosition(climbPosition + TA_Point(0, velocity.y));
        bool collisionMoved = checkPawnCollision(hitbox);

        if(collision != collisionMoved) {
            float left = 0, right = 1;
            while(right - left > TA::epsilon) {
                float mid = (left + right) / 2;
                hitbox.setPosition(climbPosition + TA_Point(0, velocity.y * mid));
                if(checkPawnCollision(hitbox) == collision) {
                    left = mid;
                } else {
                    right = mid;
                }
            }

            hitbox.setPosition(climbPosition + TA_Point(0, velocity.y * left));
            if(checkPawnCollision(hitbox)) {
                climbPosition.y += velocity.y * right;
            } else {
                climbPosition.y += velocity.y * left;
            }

            hitbox.setPosition(climbPosition + TA_Point(0, 0.01));
            int collisionFlags = links.objectSet->checkCollision(hitbox);
            if(collisionFlags & TA_COLLISION_MOVING_PLATFORM) {
                return;
            }

            TA_Point delta = climbPosition - position + TA_Point(0, 0.01);
            int flags = getCollisionFlags(topLeft + delta, bottomRight + delta);
            if(flags == TA_GROUND_COLLISION) {
                ground = true;
                jump = spring = wall = false;
                if(water && !(collisionFlags & TA_COLLISION_WATER)) {
                    links.objectSet->spawnObject<TA_Splash>(position + TA_Point(14, (height == 32 ? 6 : 22)));
                    waterSound.play();
                    water = false;
                }

                climbTime = 0;
                if(height == 32) {
                    setAnimation("climb_high");
                    state = STATE_CLIMB_HIGH;
                } else {
                    setAnimation("climb");
                    state = STATE_CLIMB_LOW;
                }
                if(links.controller->getDirection() == TA_DIRECTION_LEFT) {
                    position = climbPosition + TA_Point(13, height);
                } else {
                    position = climbPosition + TA_Point(-13, height);
                }
                velocity.y = 0;
            }
        }
    };

    updateClimbPosition(32);
    updateClimbPosition(16);
}

void TA_Character::updateSpringCollision() {
    TA_Rect hitbox;
    hitbox.setPosition(position);
    hitbox.setRectangle({topLeft.x + 0.01, bottomRight.y - 0.01}, bottomRight + TA_Point(-0.01, 0.01));

    if(links.objectSet->checkCollision(hitbox) & TA_COLLISION_SPRING) {
        jumpSound.play();
        jumpSpeed = springYsp;
        jump = spring = true;
        jumpReleased = true;
        jumpTime = 0;
        ground = false;
    }
}

void TA_Character::updateWaterCollision() {
    TA_Rect hitbox;
    hitbox.setPosition(position);
    hitbox.setRectangle(topLeft, bottomRight);

    bool newWater = (links.objectSet->checkCollision(hitbox) & TA_COLLISION_WATER);
    if(water != newWater && newWater == (velocity.y > 0) && !ground) {
        links.objectSet->spawnObject<TA_Splash>(position + TA_Point(14, 22));
        waterSound.play();
    }
    if(water && !newWater && velocity.y < 0 && !spring) {
        jumpSpeed = (remoteRobot ? remoteRobotJumpSpeed : jmp) * 0.8;
        jumpReleased = false;
        jump = true;
        jumpTime = 0;
    }
    water = newWater;
}
