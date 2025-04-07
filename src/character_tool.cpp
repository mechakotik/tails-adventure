#include <algorithm>
#include "character.h"
#include "hud.h"
#include "objects/bomb.h"

void TA_Character::updateTool() {
    if(usingNightVision) {
        updateNightVision();
    }
    if(!ground || !links.controller->isPressed(TA_BUTTON_B)) {
        usingSpeedBoots = false;
    }
    if(!links.controller->isJustPressed(TA_BUTTON_B)) {
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
            spawnBomb(BOMB_REGULAR);
            break;
        case TOOL_REMOTE_BOMB:
            spawnBomb(BOMB_REMOTE);
            break;
        case TOOL_NAPALM_BOMB:
            spawnBomb(BOMB_NAPALM);
            break;
        case TOOL_TRIPLE_BOMB:
            spawnBomb(BOMB_TRIPLE);
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
        case TOOL_NIGHT_VISION:
            initNightVision();
            break;
        default:
            damageSound.play();
            break;
    }
}

void TA_Character::spawnBomb(BombType type) {
    auto spawn = [&](TA_Point position, bool flip, TA_BombMode mode) {
        if(type == BOMB_REMOTE) {
            links.objectSet->spawnObject<TA_RemoteBomb>(position, flip, mode);
        } else if(type == BOMB_NAPALM) {
            links.objectSet->spawnObject<TA_NapalmBomb>(position, flip, mode);
        } else if(type == BOMB_TRIPLE) {
            links.objectSet->spawnObject<TA_TripleBomb>(position, flip, mode);
        } else {
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
    } else if(jump) {
        spawn(position + TA_Point((flip ? 24 : 9), 7), flip, TA_BOMB_MODE_AIR);
        jump = false;
        setAnimation("throw_air");
    } else if(crouch) {
        spawn(position + TA_Point((flip ? 11 : 22), 21), flip, TA_BOMB_MODE_CROUCH);
        setAnimation("throw_crouch");
        state = STATE_THROW_BOMB;
    } else if(ground) {
        spawn(position + TA_Point((flip ? 26 : 7), 11), flip, TA_BOMB_MODE_DEFAULT);
        setAnimation("throw");
        state = STATE_THROW_BOMB;
    }
}

void TA_Character::spawnRemoteRobot() {
    const std::vector<std::string> bossLevels{"maps/pf/pf3", "maps/pm/pm4", "maps/ci/ci3"};

    if(!ground || std::find(bossLevels.begin(), bossLevels.end(), TA::levelPath) != bossLevels.end()) {
        damageSound.play();
        return;
    }

    hitbox.setPosition(position + TA_Point(0, 0.01));
    if(links.objectSet->checkCollision(hitbox) & (TA_COLLISION_MOVING_PLATFORM | TA_COLLISION_DAMAGE)) {
        damageSound.play();
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

void TA_Character::initHammer() {
    if(ground || (jump && !helitail)) {
        setAnimation("hammer");
        state = STATE_HAMMER;
        hammerSound.play();
        if(!ground) {
            jump = helitail = false;
            velocity = {0, hammerFallSpeed};
        }
    }
}

void TA_Character::updateHammer() {
    if(!ground) {
        TA_Point topLeft{18, 12}, bottomRight{30, 39};
        int flags = moveAndCollide(topLeft, bottomRight, velocity * TA::elapsedTime);
        if(flags & TA_GROUND_COLLISION) {
            ground = true;
        }
    }

    setPosition(position);
    hitbox.setPosition(position);
    hammerHitbox.setPosition(position);
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
    hammerHitbox.setRectangle(topLeft, topLeft + TA_Point(18, 18));
    updateFollowPosition();

    if(!isAnimated()) {
        state = STATE_NORMAL;
    }
}

void TA_Character::resetInstaShield() {}

void TA_Character::initTeleport() {
    state = STATE_TELEPORT;
    teleportSound.play();
    setAnimation("teleport");
    velocity = {0, 0};
    teleportTime = 0;
}

void TA_Character::updateTeleport() {
    teleportTime += TA::elapsedTime;
    if(teleportTime > teleportInitTime) {
        velocity.y -= grv * TA::elapsedTime;
        position = position + velocity * TA::elapsedTime;
    }
    setPosition(position);
}

bool TA_Character::isTeleported() {
    return state == STATE_TELEPORT && !TA::sound::isPlaying(TA_SOUND_CHANNEL_SFX3);
}

void TA_Character::initNightVision() {
    if(!links.objectSet->isNight() || usingNightVision || nightVisionTimer > nightVisionActivateTime) {
        damageSound.play();
        return;
    }
    usingNightVision = true;
    nightVisionTimer = 0;
}

void TA_Character::updateNightVision() {
    nightVisionTimer += TA::elapsedTime;
    if(nightVisionTimer > nightVisionActivateTime) {
        nightVisionSound.play();
        links.tilemap->setLayerAlpha(links.tilemap->getNumLayers() - 1, 0);
        usingNightVision = false;
    } else {
        links.tilemap->setLayerAlpha(links.tilemap->getNumLayers() - 1, 255 - 255 * nightVisionTimer / nightVisionActivateTime);
    }
}

void TA_Character::changeMusic() {
    const std::vector<std::string> music{"sound/bf.vgm", "sound/boss.vgm", "sound/cc.vgm", "sound/cf.vgm",
        "sound/final.vgm", "sound/house.vgm", "sound/lr.vgm", "sound/map.vgm", "sound/password.vgm", "sound/pf.vgm",
        "sound/pm.vgm", "sound/sea_fox.vgm", "sound/radio.vgm", "sound/title.vgm", "sound/vt.vgm"};

    // TODO: don't play current music
    int pos = TA::random::next() % (int)music.size();
    TA::sound::playMusic(music[pos]);
}
