#include "character.h"
#include "hud.h"
#include "objects/bomb.h"

void TA_Character::updateTool()
{
    if(!ground || !controller.isPressed(TA_BUTTON_B)) {
        usingSpeedBoots = false;
    }
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
        case TOOL_HAMMER:
            initHammer();
            break;
        case TOOL_TELEPORT_DEVICE:
            initTeleport();
            break;
        case TOOL_RADIO:
            changeMusic();
            break;
        case TOOL_SPEED_BOOTS:
            usingSpeedBoots = true;
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

void TA_Character::initHammer()
{
    if(ground || (jump && !helitail)) {
        setAnimation("hammer");
        state = STATE_HAMMER;
        if(!ground) {
            jump = helitail = false;
            velocity = {0, hammerFallSpeed};
        }
    }
}

void TA_Character::updateHammer()
{
    if(!ground) {
        TA_Point topLeft{18, 12}, bottomRight{30, 39};
        int flags = moveAndCollide(topLeft, bottomRight, velocity * TA::elapsedTime);
        if(flags & TA_GROUND_COLLISION) {
            ground = true;
        }
    }

    setPosition(position);
    hitbox.setPosition(position);
    TA_Point topLeft;

    switch(getAnimationFrame()) {
        case 0:
            topLeft = {-2, 12};
            break;
        case 1:
            topLeft = {12, -1};
            break;
        case 2:
            topLeft = {29, 4};
            break;
        case 3:
            topLeft = {32, 10};
            break;
    }

    if(flip) {
        topLeft.x = 30 - topLeft.x;
    }
    hitbox.setRectangle(topLeft, topLeft + TA_Point(18, 18));
    
    if(!isAnimated()) {
        state = STATE_NORMAL;
    }
}

void TA_Character::resetInstaShield()
{
    
}

void TA_Character::initTeleport()
{
    state = STATE_TELEPORT;
    teleportSound.play();
    setAnimation("teleport");
    velocity = {0, 0};
    teleportTime = 0;
}

void TA_Character::updateTeleport()
{
    teleportTime += TA::elapsedTime;
    if(teleportTime > teleportInitTime) {
        velocity.y -= grv * TA::elapsedTime;
        position = position + velocity * TA::elapsedTime;
    }
    setPosition(position);
}

bool TA_Character::isTeleported()
{
    double cameraY = links.camera->getPosition().y;
    return state == STATE_TELEPORT && !TA::sound::isPlaying(TA_SOUND_CHANNEL_SFX3);
}

void TA_Character::changeMusic()
{
    struct LevelMusic {
        std::string begin, loop;
    };

    std::vector<LevelMusic> music;
    music.push_back({"sound/map_begin.ogg", "sound/map_loop.ogg"});
    music.push_back({"", "sound/house.ogg"});
    music.push_back({"sound/pf_begin.ogg", "sound/pf_loop.ogg"});
    music.push_back({"", "sound/vt.ogg"});
    music.push_back({"sound/pm_begin.ogg", "sound/pm_loop.ogg"});

    while(true) {
        int pos = TA::random::next() % (int)music.size();
        if(links.objectSet->getCurrentLoopMusic() == music[pos].loop) {
            continue;
        }
        links.objectSet->setMusic(music[pos].begin, music[pos].loop);
        break;
    }
}
