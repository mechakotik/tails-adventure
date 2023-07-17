#include "character.h"
#include "hud.h"
#include "objects/bomb.h"

void TA_Character::updateTool()
{
    if(!controller.isJustPressed(TA_BUTTON_B)) {
        return;
    }
    if(hurt) {
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
            spawnBomb(false);
            break;
        case TOOL_REMOTE_BOMB:
            spawnBomb(true);
            break;
        case TOOL_REMOTE_ROBOT:
            spawnRemoteRobot();
            break;
        case TOOL_INSTA_SHIELD:
            spawnInstaShield();
            break;
        default:
            damageSound.play();
            break;
    }
}

void TA_Character::spawnBomb(bool remote)
{
    auto spawn = [&] (TA_Point position, bool flip, TA_BombMode mode) {
        if(remote) {
            links.objectSet->spawnObject<TA_RemoteBomb>(position, flip, mode);
        }
        else {
            links.objectSet->spawnObject<TA_Bomb>(position, flip, mode);
        }
    };

    bombDestroySignal = false;
    if(links.objectSet->hasCollisionType(TA_COLLISION_BOMB)) {
        bombDestroySignal = true;
        return;
    }
    if(helitail) {
        spawn(position + TA_Point((flip ? 14 : 20), 27), flip, TA_BOMB_MODE_HELITAIL);
        setAnimation("throw_helitail");
    }
    else if(jump) {
        spawn(position + TA_Point((flip ? 24 : 9), 7), flip, TA_BOMB_MODE_AIR);
        jump = false;
        setAnimation("throw_air");
    }
    else if(crouch) {
        spawn(position + TA_Point((flip ? 11 : 22), 21), flip, TA_BOMB_MODE_CROUCH);
        setAnimation("throw_crouch");
        state = STATE_THROW_BOMB;
    }
    else if(ground) {
        spawn(position + TA_Point((flip ? 26 : 7), 11), flip, TA_BOMB_MODE_DEFAULT);
        setAnimation("throw");
        state = STATE_THROW_BOMB;
    }
}

void TA_Character::spawnRemoteRobot()
{
    if(!ground) {
        return;
    }
    hitbox.setPosition(position + TA_Point(0, 0.01));
    if(links.objectSet->checkCollision(hitbox) & TA_COLLISION_MOVING_PLATFORM) {
        return;
    }
    
    remoteRobot = true;
    remoteRobotInitialPosition = position;
    remoteRobotControlSprite.setPosition(remoteRobotInitialPosition);
    remoteRobotControlSprite.setFlip(flip);
    timer = 0;
    setAnimation("remote_robot_idle");
    state = STATE_REMOTE_ROBOT_INIT;
}

void TA_Character::spawnInstaShield()
{
    if(instaShieldTime < instaShieldCooldownTime) {
        return;
    }
    instaShieldSprite.setAnimation("shield");
    instaShieldTime = 0;
    instaShieldSound.play();
}

void TA_Character::updateInstaShield()
{
    if(instaShieldSprite.isAnimated()) {
        instaShieldSprite.setPosition(position + TA_Point(8, 8));
        instaShieldSprite.setAlpha(255);
    }
    else {
        instaShieldTime += TA::elapsedTime;
        instaShieldSprite.setAlpha(0);
    }
}

void TA_Character::resetInstaShield()
{
    if(instaShieldSprite.isAnimated()) {
        instaShieldTime = instaShieldCooldownTime;
    }
}
