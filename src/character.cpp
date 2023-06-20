#include <algorithm>
#include <cmath>
#include "character.h"
#include "tools.h"
#include "error.h"
#include "gamepad.h"
#include "save.h"
#include "hud.h"
#include "objects/bomb.h"

void TA_Character::load(TA_Links newLinks)
{
    links = newLinks;
    controller.load();

    jumpSound.load("sound/jump.ogg", TA_SOUND_CHANNEL_SFX1);
    remoteRobotStepSound.load("sound/remote_robot_step.ogg", TA_SOUND_CHANNEL_SFX1);
    areaBeginSound.play();

    TA_Pawn::load("tails/tails.png", 48, 48);
    loadAnimationsFromFile("tails/animations.xml");
    setCamera(links.camera);

    remoteRobotControlSprite.load("tails/tails.png", 48, 48);
    remoteRobotControlSprite.loadAnimationsFromFile("tails/animations.xml");
    remoteRobotControlSprite.setAnimation("control_remote_robot");
    remoteRobotControlSprite.setCamera(links.camera);

    rings = TA::save::getSaveParameter("rings");
}

void TA_Character::handleInput()
{
    //TA::printLog("%f %f", position.x, position.y);
    controller.setAnalogStick(helitail);
    controller.update();
    if(state != STATE_NORMAL) {
        return;
    }

    physicsStep();
    double prevX = position.x;
    updateCollisions();
    deltaX = position.x - prevX;
}

void TA_Character::update()
{
    if(state == STATE_CLIMB_LOW || state == STATE_CLIMB_HIGH) {
        updateClimbAnimation();
        return;
    }
    if(state == STATE_THROW_BOMB) {
        updateThrowAnimation();
        return;
    }
    if(state == STATE_DEAD) {
        invincibleTimeLeft -= TA::elapsedTime;
        return;
    }
    if(state == STATE_REMOTE_ROBOT_INIT) {
        timer += TA::elapsedTime;
        setAlpha(255 * (timer / remoteRobotInitTime));
        if(timer > remoteRobotInitTime) {
            state = STATE_NORMAL;
        }
        return;
    }
    if(state == STATE_REMOTE_ROBOT_RETURN) {
        updateRemoteRobotReturn();
        return;
    }
    if(state == STATE_UNPACK_ITEM) {
        if(remoteRobot) {
            setAnimation("remote_robot_idle");
        }
        else {
            setAnimation("unpack");
        }
        setPosition(position);
        updateFollowPosition();
        return;
    }
    if(state == STATE_RAISE_ITEM) {
        if(!isAnimated() && (remoteRobot || getCurrentFrame() == 74)) {
            state = STATE_NORMAL;
            velocity = {0, 0};
        }
        return;
    }

    setPosition(position);
    setFlip(flip);
    updateObjectCollision();
    updateTool();
    updateFollowPosition();
    if(state == STATE_THROW_BOMB || state == STATE_REMOTE_ROBOT_RETURN) {
        return;
    }
    updateAnimation();
}

void TA_Character::draw()
{
    if(remoteRobot) {
        remoteRobotControlSprite.draw();
    }
    TA_Pawn::draw();
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

    if(remoteRobot) {
        if(helitail) {
            if(!isAnimated()) {
                setAnimation("remote_robot_fly_loop");
            }
        }
        else if(ground && !TA::equal(velocity.x, 0)) {
            setAnimation("remote_robot_walk");
        }
        else {
            setAnimation("remote_robot_idle");
        }
        return;
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
            if(getAnimationName() != "throw_helitail") {
                if(helitailTime > maxHelitailTime) {
                    setAnimation("helitail_tired");
                }
                else {
                    setAnimation("helitail");
                }
            }
        }
        else if(jump) {
            if(velocity.y < 0) {
                setAnimation("jump_up");
            }
            else {
                setAnimation("jump_down");
            }
        }
        else {
            if(getAnimationName() != "throw_air") {
                setAnimation("fall");
            }
        }
    }
}

void TA_Character::updateTool()
{
    if(!controller.isJustPressed(TA_BUTTON_B)) {
        return;
    }
    if(remoteRobot) {
        if(state != STATE_REMOTE_ROBOT_INIT) {
            setAnimation("remote_robot_idle");
            state = STATE_REMOTE_ROBOT_RETURN;
        }
        return;
    }
    
    switch(links.hud->getCurrentItem()) {
        case TOOL_BOMB:
        case TOOL_REMOTE_BOMB: {
            auto spawnBomb = [&] (TA_Point position, bool flip, TA_BombMode mode) {
                if(links.hud->getCurrentItem() == TOOL_BOMB) {
                    links.objectSet->spawnObject<TA_Bomb>(position, flip, mode);
                }
                else {
                    links.objectSet->spawnObject<TA_RemoteBomb>(position, flip, mode);
                }
            };

            bombDestroySignal = false;
            if(links.objectSet->hasCollisionType(TA_COLLISION_BOMB)) {
                bombDestroySignal = true;
                break;
            }
            if(helitail) {
                spawnBomb(position + TA_Point((flip ? 14 : 20), 27), flip, TA_BOMB_MODE_HELITAIL);
                setAnimation("throw_helitail");
            }
            else if(jump) {
                spawnBomb(position + TA_Point((flip ? 24 : 9), 7), flip, TA_BOMB_MODE_AIR);
                jump = false;
                setAnimation("throw_air");
            }
            else if(crouch) {
                spawnBomb(position + TA_Point((flip ? 11 : 22), 21), flip, TA_BOMB_MODE_CROUCH);
                setAnimation("throw_crouch");
                state = STATE_THROW_BOMB;
            }
            else if(ground) {
                spawnBomb(position + TA_Point((flip ? 26 : 7), 11), flip, TA_BOMB_MODE_DEFAULT);
                setAnimation("throw");
                state = STATE_THROW_BOMB;
            }
            break;
        }

        case TOOL_REMOTE_ROBOT: {
            if(!ground) {
                break;
            }
            remoteRobot = true;
            remoteRobotInitialPosition = position;
            remoteRobotControlSprite.setPosition(remoteRobotInitialPosition);
            remoteRobotControlSprite.setFlip(flip);
            timer = 0;
            setAnimation("remote_robot_idle");
            state = STATE_REMOTE_ROBOT_INIT;
            break;
        }

        default:
            break;
    }
}

void TA_Character::updateClimbAnimation()
{
    if(!isAnimated()) {
        position = climbPosition;
        state = STATE_NORMAL;
    }
    else if(state == STATE_CLIMB_HIGH && getAnimationFrame() == 1) {
        setPosition(position.x, position.y - 8);
    }
    else if(getAnimationFrame() == 2) {
        setPosition(position.x, climbPosition.y);
    }
    updateFollowPosition();
}

void TA_Character::updateThrowAnimation()
{
    if(controller.isJustPressed(TA_BUTTON_B)) {
        bombDestroySignal = true;
    }
    if(!isAnimated()) {
        state = STATE_NORMAL;
    }
    updateFollowPosition();
}

void TA_Character::updateFollowPosition()
{
    TA_Point sourcePosition = position;
    if(state == STATE_CLIMB_LOW || state == STATE_CLIMB_HIGH) {
        sourcePosition.x = climbPosition.x;
    }
    followPosition = sourcePosition + TA_Point(22 - TA::screenWidth / 2, 26 - TA::screenHeight / 2);

    if(ground && (controller.getDirection() == TA_DIRECTION_UP || controller.getDirection() == TA_DIRECTION_DOWN)) {
        lookTime += TA::elapsedTime;
    }
    else {
        lookTime = 0;
    }
    if(lookTime > maxLookTime) {
        if(controller.getDirection() == TA_DIRECTION_UP) {
            followPosition.y -= 48;
        }
        else {
            followPosition.y += 64;
        }
    }
}

void TA_Character::setSpawnPoint(TA_Point newPosition, bool newFlip)
{
    position = newPosition;
    flip = newFlip;
    updateFollowPosition();
    links.camera->setFollowPosition(&followPosition);
}

void TA_Character::updateRemoteRobotReturn()
{
    timer = std::fmod(timer + TA::elapsedTime, 30);
    if(timer < 15) {
        setAlpha(255 - 255 * (timer / 5));
    }
    else {
        setAlpha(255 * ((timer - 15) / 5));
    }

    TA_Point velocity = remoteRobotInitialPosition - position;
    if(velocity.length() < 1) {
        position = remoteRobotInitialPosition;
        remoteRobot = false;
        jump = helitail = false;
        ground = true;
        flip = remoteRobotControlSprite.getFlip();
        state = STATE_NORMAL;
        return;
    }
    double divisor = velocity.length();
    velocity.x /= divisor;
    velocity.y /= divisor;
    position = position + velocity;

    setPosition(position);
    updateFollowPosition();
}

void TA_Character::addRings(int count)
{
    rings += count;
    TA::save::setSaveParameter("rings", rings);
}

void TA_Character::setRaiseState()
{
    state = STATE_RAISE_ITEM;
    if(remoteRobot) {
        setAnimation("remote_robot_raise");
    }
    else {
        setAnimation("raise");
    }
}

void TA_Character::setReleaseState()
{
    if(!remoteRobot) {
        setAnimation("release");
    }
}
