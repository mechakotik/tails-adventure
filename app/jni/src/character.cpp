#include <algorithm>
#include "character.h"
#include "tools.h"
#include "error.h"
#include "gamepad.h"
#include "objects/bomb.h"

void TA_Character::load(TA_GameScreenLinks newLinks)
{
    links = newLinks;
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
    //TA::printLog("%f %f", position.x, position.y);
    controller.setAnalogStick(helitail);
    controller.update();
    if(climb || throwing || dead) {
        return;
    }

    physicsStep();
    double prevX = position.x;
    updateCollisions();
    deltaX = position.x - prevX;
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
    setFlip(flip);
    updateObjectCollision();
    updateTool();
    if(throwing) {
        return;
    }
    updateAnimation();
    updateFollowPosition();
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
                links.objectSet->spawnObject<TA_Bomb>(position + TA_Point(20, 32), flip, TA_BOMB_MODE_HELITAIL);
            }
            else if(!ground) {
                links.objectSet->spawnObject<TA_Bomb>(position + TA_Point((flip ? 25 : 10), 8), flip, TA_BOMB_MODE_AIR);
            }
            else if(crouch) {
                links.objectSet->spawnObject<TA_Bomb>(position + TA_Point((flip ? 12 : 23), 20), flip, TA_BOMB_MODE_CROUCH);
            }
            else {
                links.objectSet->spawnObject<TA_Bomb>(position + TA_Point((flip ? 27 : 8), 12), flip, TA_BOMB_MODE_DEFAULT);
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

void TA_Character::setSpawnPoint(TA_Point newPosition, bool newFlip)
{
    position = newPosition;
    flip = newFlip;
    updateFollowPosition();
    links.camera->setFollowPosition(&followPosition);
}
