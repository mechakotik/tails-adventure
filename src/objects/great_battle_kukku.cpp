#include "great_battle_kukku.h"
#include <algorithm>
#include "defs.h"
#include "enemy_bomb.h"
#include "explosion.h"
#include "save.h"
#include "screen.h"
#include "sliding_bomb.h"
#include "tools.h"

void TA_GreatBattleKukku::load(TA_Point origin) {
    TA_Sprite::loadFromToml("objects/great_battle_kukku/great_battle_kukku.toml");
    this->origin = origin;
    position = origin + TA_Point(176, 96);
    updatePosition();
    setAnimation("idle");

    characterMock.loadFromToml("tails/tails.toml");
    characterMock.setCamera(objectSet->getLinks().camera);

    seaFoxMock.loadFromToml("tails/seafox.toml");
    seaFoxMock.setCamera(objectSet->getLinks().camera);
    seaFoxMock.setPosition(origin + TA_Point(332, 136));
    seaFoxMock.setAnimation("empty");

    flashSprite.loadFromToml("objects/great_battle_kukku/flash.toml");
    flashSprite.setCamera(objectSet->getLinks().camera);

    armSprite.loadFromToml("objects/great_battle_kukku/arm.toml");
    armSprite.setCamera(objectSet->getLinks().camera);

    exitBlockerSprite.loadFromToml("objects/great_battle_kukku/exit_blocker.toml");
    exitBlockerSprite.setPosition(origin + TA_Point(208, 112));
    exitBlockerSprite.setCamera(objectSet->getLinks().camera);

    captureHitbox.setRectangle({0, 2}, {24, 48});

    hitSound.load("sound/hit.ogg", TA_SOUND_CHANNEL_SFX3);
    explosionSound.load("sound/explosion.ogg", TA_SOUND_CHANNEL_SFX3);
    laughSound.load("sound/laugh.ogg", TA_SOUND_CHANNEL_SFX3);
    jumpSound.load("sound/jump.ogg", TA_SOUND_CHANNEL_SFX1);
}

bool TA_GreatBattleKukku::update() {
    switch(state) {
        case State::WAIT:
            updateWait();
            break;
        case State::CHARACTER_FLY:
            updateCharacterFly();
            break;
        case State::APPROACH:
            updateApproach();
            break;
        case State::LAUGH:
            updateLaugh();
            break;
        case State::MOVE:
            updateMove();
            break;
        case State::CROUCH_PHASE1:
            updateCrouchPhase1();
            break;
        case State::CROUCH_PHASE2:
            updateCrouchPhase2();
            break;
        case State::CAPTURE:
            updateCapture();
            break;
        case State::THROW:
            updateThrow();
            break;
        case State::DESTROYED:
            updateDestroyed();
            break;
        case State::WAIT_GROUND:
            updateWaitGround();
            break;
        case State::CHARACTER_EXIT:
            updateCharacterExit();
            break;
        case State::CHARACTER_JUMP:
            updateCharacterJump();
            break;
        case State::CHARACTER_FLY_AWAY:
            updateCharacterFlyAway();
            break;
    }

    if(state == State::MOVE || state == State::CROUCH_PHASE1 || state == State::CROUCH_PHASE2) {
        collisionType = TA_COLLISION_TARGET;
        if(state != State::MOVE) {
            collisionType |= TA_COLLISION_SOLID;
            hitbox.setRectangle({5, 16}, {19, 48});
        } else {
            hitbox.setRectangle({5, 2}, {19, 48});
        }
    } else {
        collisionType = TA_COLLISION_TRANSPARENT;
    }

    updatePosition();

    if(invincibleTimer > 0) {
        invincibleTimer -= TA::elapsedTime;
    } else if(state == State::MOVE || state == State::CROUCH_PHASE1 || state == State::CROUCH_PHASE2) {
        int flags = objectSet->checkCollision(hitbox);
        if((flags & TA_COLLISION_ATTACK) != 0) {
            hitSound.play();
            health--;
            if(health == 12) {
                if(state == State::MOVE) {
                    state = State::CROUCH_PHASE2;
                    timer = 0;
                }
                timeUntilCrouch = static_cast<float>(150 + (TA::random::next() % 150));
            }
            if(health == 0) {
                explosionSound.play();
                setAnimation("destroyed");
                timer = 0;
                state = State::DESTROYED;
            } else {
                invincibleTimer = 60;
            }
        }
    }

    if(state == State::CHARACTER_FLY_AWAY && mockPosition.y <= origin.y - 64) {
        objectSet->setTransition(TA_SCREENSTATE_CREDITS);
        return false;
    }
    return true;
}

void TA_GreatBattleKukku::draw() {
    TA_Object::draw();
    if(invincibleTimer >= 54 || (42 <= invincibleTimer && invincibleTimer <= 48)) {
        flashSprite.setPosition(position);
        flashSprite.setFrame(getCurrentFrame());
        flashSprite.draw();
    }
    seaFoxMock.draw();
    if(state == State::CHARACTER_FLY || state == State::APPROACH || state == State::LAUGH || state == State::CAPTURE ||
        state == State::THROW || state == State::CHARACTER_EXIT || state == State::CHARACTER_JUMP) {
        characterMock.draw();
    }
    if(state == State::CAPTURE || state == State::THROW) {
        armSprite.draw();
    }
    exitBlockerSprite.draw();
}

void TA_GreatBattleKukku::updateWait() {
    TA_Character* character = objectSet->getLinks().character;
    int64_t bossMask = TA::save::getSaveParameter("boss_mask");

    if(((bossMask & TA_BOSS_SPEEDY2) != 0) && character->isFlying() && character->getPosition().y < origin.y + 176) {
        state = State::CHARACTER_FLY;
        mockPosition = character->getPosition();
        characterMock.setPosition(mockPosition);
        characterMock.setAnimation("helitail");
        character->setHide(true);
    }
}

void TA_GreatBattleKukku::updateCharacterFly() {
    mockPosition.y -= TA::elapsedTime;
    updateFollowPosition();

    TA_Camera* camera = objectSet->getLinks().camera;
    camera->setFollowPosition(&followPosition, true);

    if(mockPosition.y < origin.y + 105) {
        mockPosition.y = origin.y + 105;
        characterMock.setAnimation("idle");
        TA::sound::playMusic("sound/final.vgm");
        camera->setLockPosition(origin + TA_Point(104 - (TA::screenWidth / 2), 160 - TA::screenHeight));
        camera->followLockX();
        camera->followLockY();
        state = State::APPROACH;
    } else {
        characterMock.setPosition(mockPosition);
    }
}

void TA_GreatBattleKukku::updateApproach() {
    setAnimation("walk");
    position.x -= 0.7F * TA::elapsedTime;
    if(position.x <= origin.x + 136) {
        position.x = origin.x + 136;
        setAnimation("laugh");
        laughSound.play();
        state = State::LAUGH;
    }
}

void TA_GreatBattleKukku::updateLaugh() {
    if(!isAnimated()) {
        TA_Character* character = objectSet->getLinks().character;
        character->setCharacterPosition({mockPosition.x, origin.y + 105});
        character->setHide(false);
        character->restoreFollowPosition();
        state = State::MOVE;
    }
}

void TA_GreatBattleKukku::updateMove() {
    setAnimation("walk");

    bool phase2 = (health <= 12);
    float minX = origin.x + (phase2 ? 8.0F : 112.0F);
    float maxX = origin.x + 176.0F;
    float speed = (phase2 ? 1.0F : 0.7F);

    if(moveRight) {
        position.x += speed * TA::elapsedTime;
        if(position.x >= maxX) {
            position.x = maxX;
            moveRight = false;
        }
    } else {
        position.x -= speed * TA::elapsedTime;
        if(position.x <= minX) {
            position.x = minX;
            moveRight = true;
        }
    }

    if(!phase2) {
        for(int i = 0; i < bombTimers.size(); ++i) {
            bombTimers[i] += TA::elapsedTime;
            while(bombTimers[i] >= bombPeriod) {
                bombTimers[i] -= bombPeriod;
                objectSet->spawnObject<TA_EnemyBomb>("objects/great_battle_kukku/bomb.toml", position, TA_Point(2, 3),
                    TA_Point(14, 15), bombVelocities[i] + TA_Point(moveRight ? 0.5F : 0, 0));
            }
        }
    }

    captureHitbox.setPosition(position);
    TA_Character* character = objectSet->getLinks().character;

    if(!character->isInvincible() && !character->isAttacking() &&
        (objectSet->checkCollision(captureHitbox) & TA_COLLISION_CHARACTER) != 0) {
        character->setHide(true);
        setAnimation("capture");
        characterMock.setPosition(position + TA_Point(-31, -16));
        characterMock.setAnimation("captured");
        armSprite.setPosition(position + TA_Point(-6, 12));
        armSprite.setAnimation("capture");
        captureTimer = 0;
        state = State::CAPTURE;
    }

    timer += TA::elapsedTime;
    if(timeUntilCrouch != 0 && timer >= timeUntilCrouch) {
        state = (phase2 ? State::CROUCH_PHASE2 : State::CROUCH_PHASE1);
        crouchBombsLeft = 10;
        timer = 0;
    }
    if(timeUntilCrouch == 0 || state != State::MOVE) {
        if(phase2) {
            timeUntilCrouch = static_cast<float>(150 + (TA::random::next() % 150));
        } else {
            timeUntilCrouch = static_cast<float>(600 + (TA::random::next() % 600));
        }
    }
}

void TA_GreatBattleKukku::updateCrouchPhase1() {
    setAnimation("crouch");
    timer += TA::elapsedTime;

    while(crouchBombsLeft >= 1 && timer >= 20) {
        timer -= 20;
        crouchBombsLeft--;
        if(crouchBombsLeft % 2 == 1) {
            objectSet->spawnObject<TA_SlidingBomb>(position + TA_Point(-8, 32), -2, nullptr);
        } else {
            objectSet->spawnObject<TA_SlidingBomb>(position + TA_Point(16, 32), 2, nullptr);
        }
    }

    if(crouchBombsLeft == 0) {
        state = State::MOVE;
        timer = 0;
    }
}

void TA_GreatBattleKukku::updateCrouchPhase2() {
    setAnimation("crouch");

    float prevTimer = timer;
    timer += TA::elapsedTime;

    if(prevTimer < 40 && timer >= 40) {
        objectSet->spawnObject<TA_EnemyBomb>("objects/great_battle_kukku/bomb.toml", position + TA_Point(8, 0),
            TA_Point(2, 3), TA_Point(14, 15), TA_Point(-1.25F, -4.5F));
        objectSet->spawnObject<TA_EnemyBomb>("objects/great_battle_kukku/bomb.toml", position + TA_Point(8, 0),
            TA_Point(2, 3), TA_Point(14, 15), TA_Point(-1.75F, -3.75F));
        objectSet->spawnObject<TA_EnemyBomb>("objects/great_battle_kukku/bomb.toml", position + TA_Point(8, 0),
            TA_Point(2, 3), TA_Point(14, 15), TA_Point(1.25F, -4.5F));
        objectSet->spawnObject<TA_EnemyBomb>("objects/great_battle_kukku/bomb.toml", position + TA_Point(8, 0),
            TA_Point(2, 3), TA_Point(14, 15), TA_Point(1.75F, -3.75F));
    }

    if(timer >= 60) {
        state = State::MOVE;
        timer = 0;
    }
}

void TA_GreatBattleKukku::updateCapture() {
    captureTimer += TA::elapsedTime;
    if(captureTimer >= 180) {
        mockPosition = position + TA_Point(-31, -16);
        throwTargetX = (mockPosition.x <= origin.x + 64 ? mockPosition.x + 80 : mockPosition.x - 64);
        characterMock.setAnimation("hurt");
        armSprite.setPosition(position + TA_Point(-6, 12));
        if(!TA::arguments.contains("--debug")) {
            objectSet->addRings(-4);
        }
        objectSet->getLinks().character->setHurt();
        state = State::THROW;
    }

    float offset = std::sin(captureTimer / 180.0F * TA::pi * 2 * 6);
    characterMock.setPosition(position + TA_Point(-31, -16) + TA_Point(offset * -3, offset * 5));
    armSprite.setPosition(position + TA_Point(-6, 12) + TA_Point(offset * -3, offset * 5));
}

void TA_GreatBattleKukku::updateThrow() {
    bool quit = false;

    if(mockPosition.x <= throwTargetX) {
        mockPosition.x += 3 * TA::elapsedTime;
        mockPosition.y -= 2 * TA::elapsedTime;
        characterMock.setPosition(mockPosition);
        if(mockPosition.x > throwTargetX) {
            quit = true;
        }
    } else {
        mockPosition.x -= 3 * TA::elapsedTime;
        mockPosition.y -= 2 * TA::elapsedTime;
        characterMock.setPosition(mockPosition);
        if(mockPosition.x < throwTargetX) {
            quit = true;
        }
    }

    if(quit) {
        TA_Character* character = objectSet->getLinks().character;
        character->setHide(false);
        character->setCharacterPosition(mockPosition);
        state = State::MOVE;
    }
}

void TA_GreatBattleKukku::updateDestroyed() {
    if(!isAnimated()) {
        TA::sound::playMusic("sound/bf.vgm");
        setAlpha(0);
        state = State::WAIT_GROUND;
        return;
    }

    soundTimer += TA::elapsedTime;
    while(soundTimer >= 40) {
        soundTimer -= 40;
        explosionSound.play();
    }
    timer += TA::elapsedTime;
    while(timer >= 6) {
        timer -= 6;
        int x = static_cast<int>(position.x) - 8 + static_cast<int>(TA::random::next() % 24);
        int y = static_cast<int>(position.y) + 24 + static_cast<int>(TA::random::next() % 16);
        objectSet->spawnObject<TA_Explosion>(TA_Point(x, y), 0, TA_EXPLOSION_NEUTRAL);
    }
}

void TA_GreatBattleKukku::updateWaitGround() {
    TA_Character* character = objectSet->getLinks().character;
    if(!character->isOnGround()) {
        return;
    }

    mockPosition = character->getPosition();
    character->setHide(true);
    characterMock.setPosition(mockPosition);

    TA_Camera* camera = objectSet->getLinks().camera;
    updateFollowPosition();
    camera->setFollowPosition(&followPosition, true);
    camera->unlock();
    timer = 0;
    state = State::CHARACTER_EXIT;
}

void TA_GreatBattleKukku::updateCharacterExit() {
    timer = std::min(32.0F, timer + TA::elapsedTime);
    exitBlockerSprite.setPosition(origin + TA_Point(208, 112 - timer));

    if(!mockMoved && !objectSet->getLinks().camera->isStable()) {
        characterMock.setAnimation("idle");
        return;
    }
    mockMoved = true;
    characterMock.setAnimation("walk");
    mockPosition.x += TA::elapsedTime;
    if(mockPosition.x >= origin.x + 280) {
        mockPosition.x = origin.x + 280;
        mockYSpeed = -2.5F;
        jumpSound.play();
        state = State::CHARACTER_JUMP;
    }
    characterMock.setPosition(mockPosition);
    updateFollowPosition();
}

void TA_GreatBattleKukku::updateCharacterJump() {
    mockYSpeed += 0.125F * TA::elapsedTime;
    if(mockYSpeed < 0) {
        characterMock.setAnimation("jump_up");
    } else {
        characterMock.setAnimation("jump_down");
    }

    mockPosition += TA_Point(1, mockYSpeed) * TA::elapsedTime;
    if(mockPosition.y >= origin.y + 120) {
        mockPosition.y = origin.y + 136;
        mockPosition.x += 8;
        mockYSpeed /= 2;
        state = State::CHARACTER_FLY_AWAY;
    }

    characterMock.setPosition(mockPosition);
}

void TA_GreatBattleKukku::updateCharacterFlyAway() {
    mockYSpeed -= 0.125F * TA::elapsedTime;
    mockYSpeed = std::max(mockYSpeed, -2.0F);
    mockPosition += TA_Point(1.5F, mockYSpeed) * TA::elapsedTime;
    seaFoxMock.setPosition(mockPosition);
    seaFoxMock.setAnimation("idle");
}

void TA_GreatBattleKukku::updateFollowPosition() {
    followPosition = mockPosition + TA_Point(22 - (TA::screenWidth / 2), 26 - (TA::screenHeight / 2));
}
