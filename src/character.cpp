#include <algorithm>
#include <cmath>
#include "character.h"
#include "tools.h"
#include "error.h"
#include "gamepad.h"
#include "save.h"
#include "hud.h"

void TA_Character::load(TA_Links newLinks)
{
    links = newLinks;
    controller.load();

    jumpSound.load("sound/jump.ogg", TA_SOUND_CHANNEL_SFX1);
    remoteRobotStepSound.load("sound/remote_robot_step.ogg", TA_SOUND_CHANNEL_SFX1);
    flySound.load("sound/fly.ogg", TA_SOUND_CHANNEL_SFX1);
    remoteRobotFlySound.load("sound/remote_robot_fly.ogg", TA_SOUND_CHANNEL_SFX1);
    damageSound.load("sound/damage.ogg", TA_SOUND_CHANNEL_SFX1);
    instaShieldSound.load("sound/insta_shield.ogg", TA_SOUND_CHANNEL_SFX3);
    deathSound.load("sound/death.ogg", TA_SOUND_CHANNEL_MUSIC);
    teleportSound.load("sound/teleport.ogg", TA_SOUND_CHANNEL_SFX3);

    TA_Pawn::load("tails/tails.png", 48, 48);
    loadAnimationsFromFile("tails/animations.xml");
    setCamera(links.camera);

    remoteRobotControlSprite.load("tails/tails.png", 48, 48);
    remoteRobotControlSprite.loadAnimationsFromFile("tails/animations.xml");
    remoteRobotControlSprite.setAnimation("control_remote_robot");
    remoteRobotControlSprite.setCamera(links.camera);

    instaShieldSprite.load("objects/insta_shield.png", 32, 32);
    instaShieldSprite.loadAnimationsFromFile("objects/insta_shield.xml");
    instaShieldSprite.setCamera(links.camera);

    rings = TA::save::getSaveParameter("rings");
}

void TA_Character::handleInput()
{
    controller.setAnalogStick(helitail);
    controller.update();

    hidden = nextFrameHidden;
    if(hidden) {
        return;
    }

    if(state == STATE_THROW_BOMB) {
        setPosition(position);
        hitbox.setPosition(position);
    }
    if(state != STATE_NORMAL) {
        return;
    }

    physicsStep();
    double prevX = position.x;
    updateCollisions();
    deltaX = position.x - prevX;

    windVelocity = {0, 0};
    strongWind = false;
}

void TA_Character::update()
{
    if(hidden) {
        return;
    }

    updateInstaShield();

    if(state == STATE_TELEPORT) {
        updateTeleport();
        return;
    }

    if(state == STATE_CLIMB_LOW || state == STATE_CLIMB_HIGH) {
        updateClimbAnimation();
        return;
    }

    if(state == STATE_THROW_BOMB) {
        updateThrowAnimation();
        setPosition(position);
        hitbox.setPosition(position);
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
    if(state == STATE_THROW_BOMB || state == STATE_REMOTE_ROBOT_RETURN || state == STATE_TELEPORT) {
        return;
    }
    updateAnimation();
}

void TA_Character::draw()
{
    if(hidden) {
        return;
    }
    if(remoteRobot) {
        remoteRobotControlSprite.draw();
    }
    instaShieldSprite.draw();
    TA_Pawn::draw();
}

TA_CollisionType TA_Character::getCollisionType()
{
    if(instaShieldSprite.isAnimated()) {
        return TA_COLLISION_INSTA_SHIELD;
    }
    return TA_COLLISION_CHARACTER;
}

void TA_Character::updateAnimation()
{
    if(rings >= 0 && hurt) {
        setAlpha(255);
    }
    else if(rings >= 0 && invincibleTimeLeft > 0) {
        setAlpha(255 - 55 * invincibleTimeLeft / invincibleTime);
    }
    else {
        setAlpha(255);
    }

    if(remoteRobot) {
        setAlpha(255);
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
                if(helitailTime > getMaxHelitailTime()) {
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
        remoteRobot = false;
        state = STATE_NORMAL;
        setCharacterPosition(remoteRobotInitialPosition);
        flip = remoteRobotControlSprite.getFlip();

        setPosition(position);
        setFlip(flip);
        updateFollowPosition();
        return;
    }

    double divisor = velocity.length();
    velocity.x /= divisor;
    velocity.y /= divisor;
    position = position + velocity * TA::elapsedTime;

    setPosition(position);
    updateFollowPosition();
}

void TA_Character::addRings(int count)
{
    rings += count;
    rings = std::min(rings, getMaxRings());
    TA::save::setSaveParameter("rings", rings);
}

void TA_Character::addRingsToMaximum()
{
    rings = getMaxRings();
    TA::save::setSaveParameter("rings", rings);
}

int TA_Character::getEmeraldsCount()
{
    long long itemMask = TA::save::getSaveParameter("item_mask");
    int count = 0;
    for(int item = 29; item <= 34; item ++) {
        if(itemMask & (1ll << item)) {
            count ++;
        }
    }
    return count;
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

void TA_Character::setWindVelociy(TA_Point windVelocity)
{
    if((helitail && helitailTime > getMaxHelitailTime()) || windVelocity.y < -2.5) {
        ground = jump = helitail = false;
    }
    if(windVelocity.y > -2.5) {
        this->windVelocity = windVelocity;
    }
    else {
        velocity.y -= strongWindForce * TA::elapsedTime;
        velocity.y = std::max(velocity.y, double(-5));
        strongWind = true;
    }
}

double TA_Character::getMaxHelitailTime()
{
    if(TA::levelPath == "maps/pm/pm4") {
        return 1e6;
    }
    return 150 * (getEmeraldsCount() + 1);
}

bool TA_Character::displayFlighTimeBar()
{
    if(TA::levelPath == "maps/pm/pm4") {
        return false;
    }
    if(state == STATE_TELEPORT) {
        return false;
    }
    return helitail && !remoteRobot;
}

void TA_Character::setCharacterPosition(TA_Point position)
{
    this->position = position;
    velocity = TA_Point(0, 0);
    ground = true;
    jump = helitail = false;
    lookUp = crouch = false;
    flip = false;
    updateAnimation();
}
