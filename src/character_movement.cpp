#include "character.h"
#include "controller.h"
#include "error.h"
#include "tools.h"

void TA_Character::physicsStep() {
    if(!hurt) {
        if(TA::levelPath.substr(0, 7) == "maps/ci" &&
            (!TA::equal(windVelocity.x, 0) || !TA::equal(windVelocity.y, 0))) {
            updateWaterFlow();
        } else if(helitail) {
            updateHelitail();
        } else if(ground) {
            updateGround();
        } else {
            updateAir();
        }
    } else {
        velocity.y += grv * (water ? 0.5F : 1) * TA::elapsedTime;
        velocity.y = std::min(velocity.y, maxJumpSpeed * (water ? 0.5F : 1));
    }

    // TA::printLog("%f %f", position.x, position.y);
}

void TA_Character::initHelitail() {
    helitail = true;
    helitailTime = 0;
    if(ground) {
        velocity.y = -2;
    }
    if(remoteRobot) {
        setAnimation("remote_robot_fly_init");
    }
}

void TA_Character::updateGround() {
    horizontalMove();
    jump = spring = false;
    coyoteTime = 0;
    lookUp = (links.controller->getDirection() == TA_DIRECTION_UP);
    crouch = (links.controller->getDirection() == TA_DIRECTION_DOWN);
    if(lookUp || crouch) {
        velocity.x = 0;
    }
    if(links.controller->isJustPressed(TA_BUTTON_A)) {
        if(lookUp && (!water || remoteRobot)) {
            initHelitail();
        } else {
            jumpSound.play();
            jumpSpeed = (remoteRobot ? remoteRobotJumpSpeed : jmp) * (water ? 0.7F : 1);
            jump = true;
            jumpReleased = false;
            jumpTime = 0;
            updateAir();
        }
        ground = false;
    }
}

void TA_Character::updateAir() {
    coyoteTime += TA::elapsedTime;
    horizontalMove();

    if(jump) {
        jumpSpeed += grv * (water ? 0.5F : 1) * TA::elapsedTime;
        jumpSpeed = std::min(jumpSpeed, maxJumpSpeed);
        jumpTime += TA::elapsedTime;

        if(jump && !jumpReleased && !links.controller->isPressed(TA_BUTTON_A)) {
            jumpReleased = true;
        }
        if(jumpReleased && !spring) {
            jumpSpeed = std::max(jumpSpeed, releaseJumpSpeed);
        }

        if(water && jumpSpeed > maxJumpSpeed * 0.5F) {
            jumpSpeed = std::max(maxJumpSpeed * 0.5F, jumpSpeed - waterFriction * TA::elapsedTime);
        }
        if(spring) {
            velocity.y = std::min(maxJumpSpeed * (water ? 0.5F : 1), jumpSpeed);
        } else {
            velocity.y = std::min(maxJumpSpeed, std::max(minJumpSpeed * (water ? 0.5F : 1), jumpSpeed));
        }
        if(jump && jumpReleased && (!water || remoteRobot) && links.controller->isJustPressed(TA_BUTTON_A)) {
            initHelitail();
        }
    }

    else {
        velocity.y += grv * (water ? 0.5F : 1) * TA::elapsedTime;
        velocity.y = std::min(velocity.y, maxJumpSpeed);
        if(water && velocity.y > maxJumpSpeed * 0.5F) {
            velocity.y = std::max(maxJumpSpeed * 0.5F, velocity.y - waterFriction * TA::elapsedTime);
        }

        if(links.controller->isJustPressed(TA_BUTTON_A) && coyoteTime < maxCoyoteTime) {
            jumpSound.play();
            jumpSpeed = (remoteRobot ? remoteRobotJumpSpeed : jmp) * (water ? 0.7 : 1);
            jump = true;
            jumpReleased = false;
            jumpTime = 0;
        }
    }
}

void TA_Character::updateHelitail() {
    auto process = [&](float& x, float need) {
        if(x > need) {
            x = std::max(need, x - helitailAcc * TA::elapsedTime);
        } else {
            x = std::min(need, x + helitailAcc * TA::elapsedTime);
        }
    };

    helitailTime += TA::elapsedTime;
    TA_Point vector;
    TA_Direction direction = links.controller->getDirection();
    if(direction != TA_DIRECTION_MAX) {
        vector = links.controller->getDirectionVector();
    }
    if(!remoteRobot && helitailTime > getMaxHelitailTime()) {
        vector.y = 1;
    }

    if(direction == TA_DIRECTION_LEFT) {
        flip = true;
    } else if(direction == TA_DIRECTION_RIGHT) {
        flip = false;
    }

    float topSpeed = helitailTop * (water ? 0.7 : 1);
    process(velocity.x, vector.x * topSpeed);
    process(velocity.y, vector.y * topSpeed);

    if(links.controller->isJustPressed(TA_BUTTON_A) || (water && !remoteRobot)) {
        jump = helitail = false;
    }

    if(!TA::sound::isPlaying(TA_SOUND_CHANNEL_SFX1)) {
        if(remoteRobot) {
            remoteRobotFlySound.play();
        } else {
            flySound.play();
        }
    }
}

void TA_Character::horizontalMove() {
    TA_Direction direction = links.controller->getDirection();
    if(remoteRobot && ground && (direction == TA_DIRECTION_LEFT || direction == TA_DIRECTION_RIGHT) &&
        !TA::sound::isPlaying(TA_SOUND_CHANNEL_SFX1)) {
        remoteRobotStepSound.play();
    }

    float currentTopX = topX;
    float currentAcc = (ground ? acc : airAcc);

    if(isUsingSpeedBoots()) {
        currentTopX *= 2;
    }
    if(water) {
        currentTopX *= 0.75;
        currentAcc *= 0.5;
    }

    if(direction == TA_DIRECTION_RIGHT) {
        flip = false;
        velocity.x += currentAcc * TA::elapsedTime;
        velocity.x = std::min(velocity.x, currentTopX);
    } else if(direction == TA_DIRECTION_LEFT) {
        flip = true;
        velocity.x -= currentAcc * TA::elapsedTime;
        velocity.x = std::max(velocity.x, -currentTopX);
    } else {
        if(velocity.x > 0) {
            velocity.x = std::max(float(0), velocity.x - currentAcc * TA::elapsedTime);
        } else {
            velocity.x = std::min(float(0), velocity.x + currentAcc * TA::elapsedTime);
        }
    }
}

void TA_Character::updateWaterFlow() {
    auto addAcceleration = [&](float& speed, float neededSpeed) {
        if(speed < neededSpeed) {
            speed = std::min(neededSpeed, speed + waterFlowAcc * TA::elapsedTime);
        } else {
            speed = std::max(neededSpeed, speed - waterFlowAcc * TA::elapsedTime);
        }
    };

    coyoteTime = maxCoyoteTime + 1;
    addAcceleration(velocity.y, windVelocity.y);
    if(TA::equal(windVelocity.x, 0)) {
        horizontalMove();
    } else {
        addAcceleration(velocity.x, windVelocity.x);
    }
}
