#include <algorithm>
#include "character.h"
#include "engine/tools.h"
#include "engine/error.h"
#include "engine/object_set.h"
#include "engine/gamepad.h"
#include "objects/bomb.h"

void TA_Character::load(TA_GameScreenLinks newLinks)
{
    links = newLinks;
    position = {1064, 264};
    updateFollowPosition();
    links.camera->setFollowPosition(&followPosition);
    controller.load();

    areaBeginSound.load("sound/pf_begin.ogg", TA_SOUND_CHANNEL_MUSIC);
    areaLoopSound.load("sound/pf_loop.ogg", TA_SOUND_CHANNEL_MUSIC, true);
    jumpSound.load("sound/jump.ogg", TA_SOUND_CHANNEL_SFX);
    ringSound.load("sound/ring.ogg", TA_SOUND_CHANNEL_SFX);
    areaBeginSound.play();

    TA_Pawn::load("tails/tails.png", 48, 48);
    loadAnimationsFromFile("tails/animations.xml");
    setCamera(links.camera);
}

void TA_Character::handleInput()
{
    controller.setAnalogStick(helitail);
    controller.update();
    if(climb || throwing || dead) {
        return;
    }

    if(!hurt) {
        if(helitail) {
            updateHelitail();
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
    }
    else {
        velocity.y += grv;
    }
    updateCollisions();
}

void TA_Character::update()
{
    if(!TA::sound::isPlaying(TA_SOUND_CHANNEL_MUSIC)) {
        areaLoopSound.play();
    }

    if(climb) {
        updateClimbAnimation();
        return;
    }
    if(throwing) {
        updateThrowAnimation();
        return;
    }
    if(dead) {
        invincibleTimeLeft -= TA::elapsedTime;
        return;
    }

    setPosition(position);
    updateObjectCollision();
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
    lookUp = (controller.getDirection() == TA_DIRECTION_UP);
    crouch = (controller.getDirection() == TA_DIRECTION_DOWN);
    if(lookUp || crouch) {
        velocity.x = 0;
    }
    if(controller.isJustPressed(TA_BUTTON_A)) {
        if(lookUp) {
            helitail = true;
            helitailTime = 0;
            velocity.y = -2;
        }
        else {
            jumpSound.play();
            velocity.y = jmp;
            jump = true;
            jumpTime = 0;
            jumpReleased = false;
        }
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

bool TA_Character::checkPawnCollision(TA_Polygon &hitbox)
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
    hitbox.setRectangle(topLeft, bottomRight);

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
        velocity.y = std::max(velocity.y, double(-0.3));
        if(jump) {
            jumpReleased = true;
        }
    }

    if(!hurt && invincibleTimeLeft <= 0) {
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
            rings --;
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
                dead = true;
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

void TA_Character::updateObjectCollision()
{
    int flags;
    links.objectSet->checkCollision(hitbox, flags);
    if(flags & TA_COLLISION_RING) {
        rings ++;
        ringSound.play();
    }
}

void TA_Character::updateAnimation()
{
    if(rings >= 0 && hurt) {
        setAlpha(200);
    }
    else if(rings >= 0 && invincibleTimeLeft > 0) {
        setAlpha(255 - 55 * invincibleTimeLeft / invincibleTime);
    }
    else {
        setAlpha(255);
    }

    if(hurt) {
        setAnimation("hurt");
    }
    else if(ground) {
        if(lookUp) {
            setAnimation("look_up");
        }
        else if(crouch) {
            setAnimation("crouch");
        }
        else if(TA::equal(velocity.x, 0)) {
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
            if(helitail) {
                links.objectSet->spawnBomb(position + TA_Point(20, 32), flip, TA_BOMB_MODE_HELITAIL);
            }
            else if(!ground) {
                links.objectSet->spawnBomb(position + TA_Point((flip ? 25 : 10), 8), flip, TA_BOMB_MODE_AIR);
            }
            else if(crouch) {
                links.objectSet->spawnBomb(position + TA_Point((flip ? 12 : 23), 20), flip, TA_BOMB_MODE_CROUCH);
            }
            else {
                links.objectSet->spawnBomb(position + TA_Point((flip ? 27 : 8), 12), flip, TA_BOMB_MODE_DEFAULT);
                setAnimation("throw");
                throwing = true;
            }
            break;

        default:
            break;
    }
}

void TA_Character::updateClimbAnimation()
{
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
}

void TA_Character::updateThrowAnimation()
{
    if(!isAnimated()) {
        throwing = false;
    }
    updateFollowPosition();
}

void TA_Character::updateFollowPosition()
{
    TA_Point sourcePosition = position;
    if(climb) {
        sourcePosition.x = climbPosition.x;
    }
    followPosition = sourcePosition + TA_Point(22 - TA::screenWidth / 2, 26 - TA::screenHeight / 2);
}
