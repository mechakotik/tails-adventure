#include <algorithm>
#include "character.h"
#include "defs.h"
#include "hud.h"
#include "large_bomb.h"
#include "objects/bomb.h"
#include "tilemap.h"
#include "tools.h"

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
        case TOOL_LARGE_BOMB:
            spawnLargeBomb();
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
        case TOOL_HELMET:
            initHelmet();
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

void TA_Character::spawnLargeBomb() {
    if(ground && !links.objectSet->hasCollisionType(TA_COLLISION_BOMB)) {
        links.objectSet->spawnObject<TA_LargeBomb>(position + TA_Point(flip ? 5 : 28, 18));
    }
}

void TA_Character::spawnRemoteRobot() {
    const std::vector<std::string> bossLevels{"maps/pf/pf3", "maps/pm/pm4", "maps/ci/ci3"};

    if(!ground || std::find(bossLevels.begin(), bossLevels.end(), TA::levelPath) != bossLevels.end()) {
        damageSound.play();
        return;
    }

    hitbox.setPosition(position + TA_Point(0, 0.01));
    if(links.objectSet->checkCollision(hitbox) & (TA_COLLISION_DAMAGE | TA_COLLISION_UNSTABLE)) {
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
        auto [delta, flags] = links.objectSet->moveAndCollide(
            position, topLeft, bottomRight, velocity * TA::elapsedTime, getSolidFlags());
        position += delta;
        if(flags & TA_GROUND_COLLISION) {
            ground = true;
        }
    }

    setPosition(position);
    hitbox.setPosition(position);
    attackHitbox.setPosition(position);
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
    attackHitbox.setRectangle(topLeft, topLeft + TA_Point(18, 18));
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
        links.objectSet->disableNight();
        usingNightVision = false;
    } else {
        links.tilemap->setLayerAlpha(
            links.tilemap->getNumLayers() - 1, 255 - 255 * nightVisionTimer / nightVisionActivateTime);
    }
}

void TA_Character::initHelmet() {
    if(strongWind) {
        return;
    }
    if(water && (!TA::equal(windVelocity.x, 0) || !TA::equal(windVelocity.y, 0))) {
        return;
    }
    setAnimation("helmet_init");
    setAlpha(255);
    state = STATE_HELMET;
    quittingHelmet = false;
    attackHitbox.setRectangle({6, 20}, {34, 39});
    if(!ground) {
        jump = helitail = false;
    }
}

void TA_Character::updateHelmet() {
    if(!quittingHelmet && !links.controller->isPressed(TA_BUTTON_B)) {
        quittingHelmet = true;
        setAnimation("helmet_quit");
    }
    if(quittingHelmet && !isAnimated()) {
        state = STATE_NORMAL;
        return;
    }
    if(!isAnimated()) {
        setAnimation("helmet");
    }

    velocity.y += grv * (water ? 0.5F : 1) * TA::elapsedTime;
    velocity.y = std::min(velocity.y, maxJumpSpeed * (water ? 0.5F : 1));
    if(velocity.x > 0) {
        velocity.x = std::max(0.0F, velocity.x - (helmetAirSlowdown * TA::elapsedTime));
    } else {
        velocity.x = std::min(0.0F, velocity.x + (helmetAirSlowdown * TA::elapsedTime));
    }

    TA_Point velocityAdd{0, 0};
    if(ground) {
        int flags = links.objectSet->checkCollision(hitbox);
        conveyorBelt = false;
        if((flags & TA_COLLISION_CONVEYOR_BELT_LEFT) != 0) {
            velocityAdd.x -= 0.8F;
            conveyorBelt = true;
        }
        if((flags & TA_COLLISION_CONVEYOR_BELT_RIGHT) != 0) {
            velocityAdd.x += 0.8F;
            conveyorBelt = true;
        }
    }

    useSolidUpTiles = useMovingPlatforms = true;
    useSolidDownTiles = false;

    TA_Point topLeft{18, 20};
    TA_Point bottomRight{30, 39};
    auto [delta, flags] = links.objectSet->moveAndCollide(
        position, topLeft, bottomRight, (velocity + velocityAdd) * TA::elapsedTime, getSolidFlags(), ground);
    position += delta;

    ground = (flags & TA_GROUND_COLLISION) != 0;
    if(ground) {
        velocity = {0, 0};
    }
    if((flags & TA_WALL_COLLISION) != 0) {
        velocity.x = 0;
    }
    if((flags & TA_CEIL_COLLISION) != 0) {
        velocity.y = std::max(velocity.y, 0.0F);
    }

    if(links.controller->getDirection() == TA_DIRECTION_LEFT) {
        flip = true;
    } else if(links.controller->getDirection() == TA_DIRECTION_RIGHT) {
        flip = false;
    }

    updateSpringCollision();
    updateWaterCollision();
    if(water && (!TA::equal(windVelocity.x, 0) || !TA::equal(windVelocity.y, 0))) {
        state = STATE_NORMAL;
        return;
    }

    setPosition(position);
    setFlip(flip);
    hitbox.setPosition(position);
    attackHitbox.setPosition(position);
    updateFollowPosition();
}

void TA_Character::changeMusic() {
    // TODO: don't hardcode this?
    const std::vector<std::string> music{"sound/bf.vgm", "sound/boss.vgm", "sound/cc.vgm", "sound/cf.vgm",
        "sound/final.vgm", "sound/house.vgm", "sound/lr.vgm", "sound/map.vgm", "sound/password.vgm", "sound/pf.vgm",
        "sound/pm.vgm", "sound/lc.vgm", "sound/radio.vgm", "sound/title.vgm", "sound/vt.vgm"};

    // TODO: don't play current music
    int pos = TA::random::next() % (int)music.size();
    TA::sound::playMusic(music[pos]);
}
