#include "mecha_golem_mk2.h"
#include "explosion.h"
#include "sound.h"

void TA_MechaGolemMk2::load(TA_Point position, TA_Point enterBlockerPosition, TA_Point exitBlockerPosition) {
    this->position = position;
    this->enterBlockerPosition = enterBlockerPosition;
    this->exitBlockerPosition = exitBlockerPosition;

    bodySprite.loadFromToml("objects/mecha_golem/mk2_body.toml");
    bodySprite.setCamera(objectSet->getLinks().camera);
    bodySprite.setPosition(position);

    headSprite.loadFromToml("objects/mecha_golem/head.toml");
    headSprite.setCamera(objectSet->getLinks().camera);
    headSprite.setPosition(position + TA_Point(32, 1));
    headSprite.setAnimation("mk2_idle");
    headFlashSprite.loadFromToml("objects/mecha_golem/head.toml");
    headFlashSprite.setCamera(objectSet->getLinks().camera);
    headFlashSprite.setPosition(position + TA_Point(32, 1));

    enterBlockerSprite.loadFromToml("objects/mecha_golem/enter_blocker.toml");
    enterBlockerSprite.setCamera(objectSet->getLinks().camera);
    enterBlockerSprite.setPosition(enterBlockerPosition - TA_Point(0, enterBlockerOffset));
    exitBlockerSprite.loadFromToml("objects/mecha_golem/exit_blocker.toml");
    exitBlockerSprite.setCamera(objectSet->getLinks().camera);
    exitBlockerSprite.setPosition(exitBlockerPosition);

    hitboxVector.assign(HITBOX_MAX, HitboxVectorElement());
    hitboxVector[HITBOX_BODY].hitbox.setRectangle({8, 12}, {54, 45});
    hitboxVector[HITBOX_BODY].hitbox.setPosition(position);
    hitboxVector[HITBOX_BODY].collisionType = TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;
    hitboxVector[HITBOX_HEAD].hitbox.setRectangle({1, 2}, {22, 20});
    hitboxVector[HITBOX_HEAD].hitbox.setPosition(position + TA_Point(32, 1));
    hitboxVector[HITBOX_HEAD].collisionType = TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;

    hitboxVector[HITBOX_ENTER_BLOCKER].hitbox.setRectangle({0, 0}, {32, 32});
    hitboxVector[HITBOX_ENTER_BLOCKER].hitbox.setPosition(enterBlockerPosition - TA_Point(0, enterBlockerOffset));
    hitboxVector[HITBOX_ENTER_BLOCKER].collisionType = TA_COLLISION_SOLID;
    hitboxVector[HITBOX_EXIT_BLOCKER].hitbox.setRectangle({0, 0}, {32, 16});
    hitboxVector[HITBOX_EXIT_BLOCKER].hitbox.setPosition(exitBlockerPosition);
    hitboxVector[HITBOX_EXIT_BLOCKER].collisionType = TA_COLLISION_SOLID;

    hitSound.load("sound/hit.ogg", TA_SOUND_CHANNEL_SFX2);
    explosionSound.load("sound/explosion.ogg", TA_SOUND_CHANNEL_SFX3);
}

bool TA_MechaGolemMk2::update() {
    switch(state) {
        case State::WAIT:
            updateWait();
            break;
        case State::IDLE:
            updateIdle();
            break;
        case State::PHASE_CHANGE:
            updatePhaseChange();
            break;
        case State::BLOW:
            updateBlow();
            break;
        case State::DEFEATED:
            break;
    }

    updateDamage();
    updateEnterBlocker();
    updateExitBlocker();
    return true;
}

void TA_MechaGolemMk2::updateWait() {
    if(objectSet->getCharacterPosition().y > position.y + 64) {
        objectSet->getLinks().camera->unlock();
    } else {
        TA_Point centerPosition = position + TA_Point(96, 0);
        TA_Point lockPosition = centerPosition - TA_Point(TA::screenWidth / 2, TA::screenHeight / 2);
        objectSet->getLinks().camera->setLockPosition(lockPosition);
        if(objectSet->getLinks().camera->isLocked()) {
            TA::sound::playMusic("sound/boss.vgm");
            state = State::IDLE;
        }
    }
}

void TA_MechaGolemMk2::updateIdle() {
    if(health <= 24 && !secondPhase) {
        initPhaseChange();
        return;
    }
    if(health <= 0) {
        initBlow();
        return;
    }
}

void TA_MechaGolemMk2::initPhaseChange() {
    timer = 0;
    state = State::PHASE_CHANGE;
    secondPhase = true;
    explosionSound.play();

    for(int delay = 0; delay < static_cast<int>(phaseChangeTime * 2 / 3); delay += phaseChangeExplosionInterval) {
        TA_Point explosionPosition = position + TA_Point(30, 2);
        explosionPosition.x += static_cast<float>(TA::random::next() % 8);
        explosionPosition.y += static_cast<float>(TA::random::next() % 16);
        objectSet->spawnObject<TA_Explosion>(explosionPosition, delay, TA_EXPLOSION_NEUTRAL);
    }
}

void TA_MechaGolemMk2::updatePhaseChange() {
    timer += TA::elapsedTime;
    if(timer > phaseChangeTime) {
        timer = 0;
        headSprite.setAnimation("idle2");
        headSprite.setFlip(true);
        headFlashSprite.setFlip(true);
        state = State::IDLE;
        return;
    }

    if(timer > phaseChangeTime / 3) {
        headSprite.setAnimation("laugh");
        headSprite.setFlip(true);
        headFlashSprite.setFlip(true);
    }
}

void TA_MechaGolemMk2::initBlow() {
    timer = 0;
    state = State::BLOW;
    headSprite.setAnimation("defeated");

    hitboxVector[HITBOX_BODY].collisionType = TA_COLLISION_TRANSPARENT;
    hitboxVector[HITBOX_HEAD].collisionType = TA_COLLISION_TRANSPARENT;

    for(int delay = 0; delay < static_cast<int>(blowTime); delay += blowInterval) {
        TA_Point explosionPosition;
        explosionPosition.x = position.x + static_cast<float>(TA::random::next() % 41);
        explosionPosition.y = position.y + static_cast<float>(TA::random::next() % 30);
        objectSet->spawnObject<TA_Explosion>(explosionPosition, delay, TA_EXPLOSION_NEUTRAL);
    }
}

void TA_MechaGolemMk2::updateBlow() {
    if(!TA::sound::isPlaying(TA_SOUND_CHANNEL_SFX3)) {
        explosionSound.play();
    }

    timer += TA::elapsedTime;
    if(timer > blowTime * 2 / 3) {
        headSprite.setAlpha(0);
    }
    if(timer > blowTime) {
        initDefeated();
    }
}

void TA_MechaGolemMk2::initDefeated() {
    TA::sound::playMusic("sound/vt.vgm");
    objectSet->getLinks().camera->unlock();
    headSprite.setAlpha(0);
    headFlashSprite.setAlpha(0);
    state = State::DEFEATED;
}

void TA_MechaGolemMk2::updateDamage() {
    if(invincibleTimer <= invincibleTime) {
        invincibleTimer += TA::elapsedTime;
        return;
    }
    if(state == State::BLOW || state == State::DEFEATED) {
        return;
    }

    if((objectSet->checkCollision(hitboxVector[HITBOX_HEAD].hitbox) & TA_COLLISION_ATTACK) == 0) {
        return;
    }
    if(state == State::WAIT) {
        return;
    }

    hitSound.play();
    invincibleTimer = 0;
    health--;
}

void TA_MechaGolemMk2::updateEnterBlocker() {
    if(state == State::WAIT) {
        return;
    }

    if(state == State::DEFEATED) {
        enterBlockerOffset = std::min(32.0F, enterBlockerOffset + (TA::elapsedTime * 2));
    } else {
        enterBlockerOffset = std::max(0.0F, enterBlockerOffset - (TA::elapsedTime * 2));
    }

    enterBlockerSprite.setPosition(enterBlockerPosition - TA_Point(0, enterBlockerOffset));
    hitboxVector[HITBOX_ENTER_BLOCKER].hitbox.setPosition(enterBlockerPosition - TA_Point(0, enterBlockerOffset));
}

void TA_MechaGolemMk2::updateExitBlocker() {
    if(state == State::DEFEATED) {
        exitBlockerOffset = std::min(32.0F, exitBlockerOffset + (TA::elapsedTime * 2));
    }

    exitBlockerSprite.setPosition(exitBlockerPosition - TA_Point(exitBlockerOffset, 0));
    hitboxVector[HITBOX_EXIT_BLOCKER].hitbox.setPosition(exitBlockerPosition - TA_Point(exitBlockerOffset, 0));
}

void TA_MechaGolemMk2::draw() {
    bodySprite.draw();
    headSprite.draw();

    if(invincibleTimer < damageFlashTime * 4 && static_cast<int>(invincibleTimer / damageFlashTime) % 2 == 0) {
        headFlashSprite.setFrame(headSprite.getCurrentFrame() + 6);
        headFlashSprite.draw();
    }

    enterBlockerSprite.draw();
    exitBlockerSprite.draw();
}
