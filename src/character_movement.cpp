#include "character.h"

void TA_Character::physicsStep()
{
    if(!hurt) {
        if(helitail) {
            updateHelitail();
        }
        else if(ground) {
            updateGround();
        }
        else {
            updateAir();
        }
    }
    else {
        velocity.y += grv * TA::elapsedTime;
    }
}

void TA_Character::initHelitail()
{
    helitail = true;
    helitailTime = 0;
    if(ground) {
        velocity.y = -2;
    }
    if(remoteRobot) {
        setAnimation("remote_robot_fly_init");
    }
}

void TA_Character::updateGround()
{
    verticalMove();
    jump = spring = false;
    lookUp = (controller.getDirection() == TA_DIRECTION_UP);
    crouch = (controller.getDirection() == TA_DIRECTION_DOWN);
    if(lookUp || crouch) {
        velocity.x = 0;
    }
    if(controller.isJustPressed(TA_BUTTON_A)) {
        if(lookUp) {
            initHelitail();
        }
        else {
            jumpSound.play();
            velocity.y = (remoteRobot ? remoteRobotJumpSpeed : jmp);
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
            velocity.y = (remoteRobot? remoteRobotJumpSpeed : jmp);
        }
        else if(!controller.isPressed(TA_BUTTON_A)) {
            jumpReleased = true;
        }
    }
    if(jump && jumpReleased && !remoteRobot && controller.isJustPressed(TA_BUTTON_A)) {
        initHelitail();
    }
}

void TA_Character::updateHelitail()
{
    auto process = [&](double &x, double need) {
        if(x > need) {
            x = std::max(need, x - helitailAcc * TA::elapsedTime);
        }
        else {
            x = std::min(need, x + helitailAcc * TA::elapsedTime);
        }
    };

    helitailTime += TA::elapsedTime;
    TA_Point vector;
    TA_Direction direction = controller.getDirection();
    if(direction != TA_DIRECTION_MAX) {
        vector = controller.getDirectionVector();
    }
    if(!remoteRobot && helitailTime > maxHelitailTime) {
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

    if(controller.isJustPressed(TA_BUTTON_A)) {
        jump = helitail = false;
    }
}

void TA_Character::verticalMove()
{
    TA_Direction direction = controller.getDirection();
    if(remoteRobot && ground && (direction == TA_DIRECTION_LEFT || direction == TA_DIRECTION_RIGHT) && !TA::sound::isPlaying(TA_SOUND_CHANNEL_SFX1)) {
        remoteRobotStepSound.play();
    }

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