#include "mecha_golem.h"
#include "mecha_golem_bomb.h"
#include "explosion.h"
#include "item_box.h"
#include "save.h"
#include "transition.h"
#include <algorithm>
#include <cmath>

void TA_MechaGolem::load()
{
    if(isComplete()) {
        objectSet->spawnObject<TA_Transition>(TA_Point(510, 0), TA_Point(512, 144), 4, false);
        return;
    }

    headSprite.loadFromToml("objects/mecha_golem/head.toml");
    headFlashSprite.loadFromToml("objects/mecha_golem/head.toml");
    bodySprite.load("objects/mecha_golem/body.png");
    leftFootSprite.load("objects/mecha_golem/feet.png", 16, 11);
    rightFootSprite.load("objects/mecha_golem/feet.png", 16, 11);
    rightFootSprite.setFrame(1);
    armSprite.loadFromToml("objects/mecha_golem/arm.toml");
    armPartSprite.load("objects/mecha_golem/arm_part.png");

    TA_Camera* camera = objectSet->getLinks().camera;
    headSprite.setCamera(camera);
    headFlashSprite.setCamera(camera);
    bodySprite.setCamera(camera);
    leftFootSprite.setCamera(camera);
    rightFootSprite.setCamera(camera);
    armSprite.setCamera(camera);
    armPartSprite.setCamera(camera);

    hitSound.load("sound/hit.ogg", TA_SOUND_CHANNEL_SFX2);
    smallExplosionSound.load("sound/explosion_small.ogg", TA_SOUND_CHANNEL_SFX3);
    explosionSound.load("sound/explosion.ogg", TA_SOUND_CHANNEL_SFX3);

    position = {double(128 + TA::screenWidth - 61), 112};
    objectSet->getLinks().camera->setLockPosition({128, 0});
    hitboxVector.assign(HITBOX_MAX, HitboxVectorElement());
}

bool TA_MechaGolem::update()
{
    if(isComplete() && state != STATE_DEFEATED && state != STATE_WAIT_ITEM) {
        return false;
    }

    switch(state) {
        case STATE_IDLE:
            updateIdle();
            break;
        case STATE_WAIT:
            updateWait();
            break;
        case STATE_GO_LEFT:
        case STATE_GO_RIGHT:
            updateGo();
            break;
        case STATE_ARM_MOVE:
            updateArmMove();
            break;
        case STATE_ARM_MOVE_BACK:
            updateArmMoveBack();
            break;
        case STATE_PHASE_CHANGE:
            updatePhaseChange();
            break;
        case STATE_ARM_CIRCLE:
            updateArmCircle();
            break;
        case STATE_ARM_BITE1:
            updateArmBite1();
            break;
        case STATE_ARM_BITE2:
            updateArmBite2();
            break;
        case STATE_ARM_BITE3:
            updateArmBite3();
            break;
        case STATE_ARM_BITE4:
            updateArmBite4();
            break;
        case STATE_BLOW:
            updateBlow();
            break;
        case STATE_FALL:
            updateFall();
            break;
        case STATE_DEFEATED:
            updateDefeated();
            break;
        case STATE_WAIT_ITEM:
            if(canDoTransition()) {
                doTransition();
                return false;
            }
            break;
    }

    if(state != STATE_ARM_MOVE && state != STATE_ARM_MOVE_BACK && state != STATE_ARM_CIRCLE &&
        state != STATE_ARM_BITE1 && state != STATE_ARM_BITE2 && state != STATE_ARM_BITE3 && state != STATE_ARM_BITE4) {
        armPosition = position + TA_Point(-11, -42);
        armSprite.setAnimation("idle");
    }

    if(!secondPhase) {
        headSprite.setAnimation("idle");
    }

    if(state != STATE_WAIT) {
        previousState = state;
    }

    updateDamage();
    updateHitboxes();
    return true;
}

bool TA_MechaGolem::isComplete()
{
    long long itemMask = TA::save::getSaveParameter("item_mask");
    return itemMask & (1ll << 22);
}

void TA_MechaGolem::updateIdle()
{
    if(objectSet->getLinks().camera->isLocked()) {
        state = STATE_WAIT;
        TA::sound::playMusic("sound/boss.vgm");
    }
}

void TA_MechaGolem::updateWait()
{
    if(health <= 16 && !secondPhase) {
        initPhaseChange();
        return;
    }
    if(health <= 0) {
        initBlow();
        return;
    }

    timer += TA::elapsedTime;
    if(timer < waitTime) {
        return;
    }

    // TODO: check point to line distance here
    if(!secondPhase) {
        double distance = objectSet->getCharacterPosition().getDistance(position + TA_Point(-11, -42));
        if(distance < armMoveMaxDistance && previousState != STATE_ARM_MOVE_BACK) {
            initArmMove();
        }
        else {
            initGo();
        }
    }
    else {
        if(previousState == STATE_GO_LEFT || previousState == STATE_GO_RIGHT) {
            timer = 0;
            double characterX = objectSet->getCharacterPosition().x;
            if(TA::random::next() % 4 == 0) {
                initGo();
            }
            else if(characterX < position.x && characterX > position.x - 48) {
                state = STATE_ARM_CIRCLE;
            }
            else {
                state = STATE_ARM_BITE1;
            }
        }
        else {
            initGo();
        }
    }
}

void TA_MechaGolem::initGo()
{
    timer = 0;
    startX = position.x;
    double cameraX = objectSet->getLinks().camera->getPosition().x;
    
    if(position.x < cameraX + goBorder) {
        state = STATE_GO_RIGHT;
    }
    else if(position.x > cameraX + TA::screenWidth - goBorder - bodySprite.getWidth()) {
        state = STATE_GO_LEFT;
    }
    else if(position.x + bodySprite.getWidth() / 2 < objectSet->getCharacterPosition().x) {
        state = STATE_GO_RIGHT;
    }
    else {
        state = STATE_GO_LEFT;
    }

    if(state == STATE_GO_RIGHT) {
        headSprite.setAnimation("turn_back");
    }
}

void TA_MechaGolem::updateGo()
{
    int direction = (state == STATE_GO_LEFT ? -1 : 1);
    timer += TA::elapsedTime;

    if(timer > goTime) {
        if(state == STATE_GO_RIGHT) {
            headSprite.setAnimation("turn_forward");
        }
        position.x = startX + goDistance * direction;
        timer = 0;
        state = STATE_WAIT;
        return;
    }

    position.x = startX + goDistance * (timer / goTime) * direction;

    if(timer < goTime / 2) {
        double angle = (timer / (goTime / 2)) * TA::pi;
        double x = startX + (-cos(angle) + 1) / 2 * goDistance * direction;
        double y = sin(angle) * stepHeight;

        if(direction == -1) {
            leftFootSprite.setPosition(x + 5, position.y - 10 - y);
            rightFootSprite.setPosition(startX + 26, position.y - 10);
        }
        else {
            leftFootSprite.setPosition(startX + 5, position.y - 10);
            rightFootSprite.setPosition(x + 26, position.y - 10 - y);
        }
    }
    else {
        double angle = ((timer - goTime / 2) / (goTime / 2)) * TA::pi;
        double x = startX + (-cos(angle) + 1) / 2 * goDistance * direction;
        double y = sin(angle) * stepHeight;

        if(direction == -1) {
            leftFootSprite.setPosition((startX + goDistance * direction) + 5, position.y - 10);
            rightFootSprite.setPosition(x + 26, position.y - 10 - y);
        }
        else {
            leftFootSprite.setPosition(x + 5, position.y - 10 - y);
            rightFootSprite.setPosition((startX + goDistance * direction) + 26, position.y - 10);
        }
    }
}

void TA_MechaGolem::initArmMove()
{
    timer = 0;
    armTarget = getOptimalArmTarget();
    state = STATE_ARM_MOVE;
    armSprite.setAnimation("attack");
}

void TA_MechaGolem::updateArmMove()
{
    timer += TA::elapsedTime;
    if(timer > armMoveTime) {
        timer = 0;
        state = STATE_ARM_MOVE_BACK;
        return;
    }

    TA_Point startPosition = position + TA_Point(-11, -42);
    armPosition = startPosition + (armTarget - startPosition) * (timer / armMoveTime);
}

void TA_MechaGolem::updateArmMoveBack()
{
    timer += TA::elapsedTime;
    if(timer > armMoveTime) {
        timer = 0;
        state = STATE_WAIT;
        return;
    }

    TA_Point startPosition = position + TA_Point(-11, -42);
    armPosition = armTarget + (startPosition - armTarget) * (timer / armMoveTime);
}

TA_Point TA_MechaGolem::getOptimalArmTarget()
{
    std::vector<TA_Point> targets;
    targets.push_back(position + TA_Point(-11, -82));
    targets.push_back(position + TA_Point(-51, -42));
    targets.push_back(position + TA_Point(-43, -74));

    std::sort(targets.begin(), targets.end(), [&] (const auto &first, const auto &second) {
        TA_Point characterPosition = objectSet->getCharacterPosition();
        return (characterPosition - first).length() < (characterPosition - second).length();
    });

    return targets[0];
}

void TA_MechaGolem::initPhaseChange()
{
    timer = 0;
    state = STATE_PHASE_CHANGE;
    secondPhase = true;
    explosionSound.play();

    for(int delay = 0; delay < phaseChangeTime * 2 / 3; delay += phaseChangeExplosionInterval) {
        TA_Point explosionPosition = position + TA_Point(5, -56);
        explosionPosition.x += TA::random::next() % 8;
        explosionPosition.y += TA::random::next() % 16;
        objectSet->spawnObject<TA_Explosion>(explosionPosition, delay, TA_EXPLOSION_NEUTRAL);
    }
}

void TA_MechaGolem::updatePhaseChange()
{
    timer += TA::elapsedTime;
    if(timer > phaseChangeTime) {
        timer = 0;
        headSprite.setAnimation("idle2");
        state = STATE_WAIT;
        return;
    }

    if(timer > phaseChangeTime / 3) {
        headSprite.setAnimation("laugh");
    }
}

void TA_MechaGolem::updateArmCircle()
{
    timer += TA::elapsedTime;
    if(timer > armCircleTime) {
        timer = 0;
        state = STATE_WAIT;
        return;
    }

    TA_Point center = position + TA_Point(-20, -60);
    double radius = center.getDistance(position + TA_Point(-11, -42));

    double baseAngle = acos((position.x - 11 - center.x) / radius);
    double angle = baseAngle + (timer / armCircleTime) * TA::pi * 2;
    if(angle > TA::pi * 2) {
        angle -= TA::pi * 2;
    }

    armPosition.x = center.x + radius * cos(angle);
    armPosition.y = center.y + radius * sin(angle);
    armSprite.setAnimation("attack");
}

void TA_MechaGolem::updateArmBite1()
{
    timer += TA::elapsedTime;
    if(timer > armBite1Time) {
        timer = 0;
        state = STATE_ARM_BITE2;
        return;
    }

    armSprite.setAnimation("attack");
    TA_Point startPosition = position + TA_Point(-11, -42);
    TA_Point endPosition = position + TA_Point(-11, -82);
    armPosition = startPosition + (endPosition - startPosition) * (timer / armBite1Time);
}

void TA_MechaGolem::updateArmBite2()
{
    TA_Point startPosition = position + TA_Point(-11, -82);
    TA_Point endPosition = position + TA_Point(-32, -15);
    timer += TA::elapsedTime;

    if(timer > armBite2Time) {
        timer = 0;
        state = STATE_ARM_BITE3;
        armPosition = endPosition;
        objectSet->getLinks().camera->shake(24);
        TA::gamepad::rumble(0.75, 0, 20);

        TA_Point bombPosition;
        bombPosition.x = objectSet->getCharacterPosition().x - 8;
        bombPosition.y = -34;
        objectSet->spawnObject<TA_MechaGolemBomb>(bombPosition);

        return;
    }

    armSprite.setAnimation("bite");
    double angle = TA::pi / 2 + (timer / armBite2Time) * (timer / armBite2Time) * (TA::pi / 2);
    armPosition.x = startPosition.x + (endPosition.x - startPosition.x) * (-cos(angle));
    armPosition.y = endPosition.y + (endPosition.y - startPosition.y) * (-sin(angle));
}

void TA_MechaGolem::updateArmBite3()
{
    timer += TA::elapsedTime;
    if(timer > armBite3Time) {
        timer = 0;
        state = STATE_ARM_BITE4;
    }
}

void TA_MechaGolem::updateArmBite4()
{
    timer += TA::elapsedTime;
    if(timer > armBite4Time) {
        timer = 0;
        state = STATE_WAIT;
    }

    TA_Point startPosition = position + TA_Point(-32, -15);
    TA_Point endPosition = position + TA_Point(-11, -42);
    armPosition = startPosition + (endPosition - startPosition) * (timer / armBite4Time);
}

void TA_MechaGolem::initBlow()
{
    TA::sound::fadeOutMusic(0);
    objectSet->spawnObject<TA_Explosion>(position + TA_Point(-11, -42), 32, TA_EXPLOSION_NEUTRAL);
    objectSet->spawnObject<TA_Explosion>(position + TA_Point(-7, -38), 36, TA_EXPLOSION_NEUTRAL);
    objectSet->spawnObject<TA_Explosion>(position + TA_Point(-3, -34), 40, TA_EXPLOSION_NEUTRAL);

    objectSet->spawnObject<TA_Explosion>(position + TA_Point(5, -12), 80);
    objectSet->spawnObject<TA_Explosion>(position + TA_Point(26, -12), 80);

    timer = 0;
    state = STATE_BLOW;
}

void TA_MechaGolem::updateBlow()
{
    double prev = timer;
    timer += TA::elapsedTime;

    if(prev <= 32 && timer > 32) {
        smallExplosionSound.play();
    }
    if(prev <= 80 && timer > 80) {
        smallExplosionSound.play();
    }

    if(timer > 90) {
        timer = 0;
        state = STATE_FALL;
    }
}

void TA_MechaGolem::updateFall()
{
    speed += gravity * TA::elapsedTime;
    position.y += speed * TA::elapsedTime;

    if(position.y >= 123) {
        position.y = 123;
        state = STATE_DEFEATED;
        headSprite.setAnimation("defeated");

        for(int delay = 0; delay < defeatedTime; delay += defeatedExplosionInterval) {
            TA_Point explosionPosition;
            explosionPosition.x = position.x + TA::random::next() % 41;
            explosionPosition.y = position.y - 57 + TA::random::next() % 30;
            objectSet->spawnObject<TA_Explosion>(explosionPosition, delay, TA_EXPLOSION_NEUTRAL);
        }
    }
}

void TA_MechaGolem::updateDefeated()
{
    if(timer > defeatedTime) {
        state = STATE_WAIT_ITEM;
        return;
    }
    double prev = timer;
    timer += TA::elapsedTime;
    
    if(!TA::sound::isPlaying(TA_SOUND_CHANNEL_SFX3)) {
        explosionSound.play();
    }

    if(prev < defeatedTime / 3 && timer >= defeatedTime / 3) {
        objectSet->spawnObject<TA_ItemBox>(position + TA_Point(8, -32), TA_Point(-1, -2), 22, " anti-air \n missile  ");
    }
}

bool TA_MechaGolem::canDoTransition()
{
    return isComplete() && !objectSet->getLinks().character->isGettingItem();
}

void TA_MechaGolem::doTransition()
{
    TA::save::setSaveParameter("map_selection", 4);
    TA::save::setSaveParameter("seafox", 0);
    objectSet->setTransition(TA_SCREENSTATE_MAP);
}

void TA_MechaGolem::updateDamage()
{
    if(invincibleTimer <= invincibleTime) {
        invincibleTimer += TA::elapsedTime;
        return;
    }

    if((objectSet->checkCollision(hitboxVector[HITBOX_WEAK].hitbox) & TA_COLLISION_ATTACK) == 0) {
        return;
    }
    if(state == STATE_BLOW || state == STATE_FALL || state == STATE_DEFEATED) {
        return;
    }

    hitSound.play();
    invincibleTimer = 0;
    health --;
}

void TA_MechaGolem::updateHitboxes()
{
    // TODO: add hitbox to block bombs from back
    hitboxVector[HITBOX_WALL_LEFT].hitbox.setRectangle(TA_Point(112, 0), TA_Point(128, 112));
    hitboxVector[HITBOX_WALL_RIGHT].hitbox.setRectangle(TA_Point(128 + TA::screenWidth, 0), TA_Point(144 + TA::screenWidth, 112));
    hitboxVector[HITBOX_WALL_LEFT].collisionType = hitboxVector[HITBOX_WALL_RIGHT].collisionType = (state == STATE_IDLE ? TA_COLLISION_TRANSPARENT : TA_COLLISION_SOLID);
    
    int type = (state == STATE_BLOW || state == STATE_FALL || state == STATE_DEFEATED || state == STATE_WAIT_ITEM ? TA_COLLISION_TRANSPARENT : TA_COLLISION_DAMAGE | TA_COLLISION_TARGET);
    hitboxVector[HITBOX_BODY].hitbox.setRectangle(position + TA_Point(5, -44), position + TA_Point(52, -16));
    hitboxVector[HITBOX_WEAK].hitbox.setRectangle(position + TA_Point(8, -55), position + TA_Point(24, -42));
    hitboxVector[HITBOX_BODY].collisionType = hitboxVector[HITBOX_WEAK].collisionType = type;

    type = (state == STATE_BLOW || state == STATE_FALL || state == STATE_DEFEATED || state == STATE_WAIT_ITEM ? TA_COLLISION_TRANSPARENT : TA_COLLISION_DAMAGE);
    hitboxVector[HITBOX_ARM].hitbox.setRectangle(armPosition + TA_Point(4, 4), armPosition + TA_Point(12, 12));
    hitboxVector[HITBOX_ARM].collisionType = type;
}

void TA_MechaGolem::draw()
{
    headSprite.setPosition(position + TA_Point(5, -56));
    bodySprite.setPosition(position + TA_Point(0, -57));

    if(state != STATE_GO_LEFT && state != STATE_GO_RIGHT) {
        leftFootSprite.setPosition(position + TA_Point(5, -10));
        rightFootSprite.setPosition(position + TA_Point(26, -10));
    }

    headFlashSprite.setPosition(headSprite.getPosition());
    headFlashSprite.setFrame(headSprite.getCurrentFrame() + 5);

    bodySprite.draw();
    if(!(state == STATE_WAIT_ITEM || (state == STATE_DEFEATED && timer > defeatedTime * 2 / 3))) {
        headSprite.draw();
    }

    if(!(state == STATE_FALL || state == STATE_DEFEATED || state == STATE_WAIT_ITEM || (state == STATE_BLOW && timer > 88))) {
        leftFootSprite.draw();
        rightFootSprite.draw();
    }
    
    drawArm();

    if(invincibleTimer < damageFlashTime * 4 && int(invincibleTimer / damageFlashTime) % 2 == 0) {
        headFlashSprite.draw();
    }
}

void TA_MechaGolem::drawArm()
{
    if(state == STATE_FALL || state == STATE_DEFEATED || state == STATE_WAIT_ITEM || (state == STATE_BLOW && timer > 40)) {
        return;
    }

    armSprite.setPosition(armPosition);
    armSprite.draw();

    TA_Point startAdd = TA_Point(2, -26);
    TA_Point endAdd = (armPosition - position) + TA_Point(4, 8);

    for(double part : {0.25, 0.5, 0.75}) {
        TA_Point currentAdd = startAdd + (endAdd - startAdd) * part;
        currentAdd.x = round(currentAdd.x);
        currentAdd.y = round(currentAdd.y);
        armPartSprite.setPosition(position + currentAdd);
        armPartSprite.draw();
    }
}
