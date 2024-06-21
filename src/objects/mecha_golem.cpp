#include "mecha_golem.h"
#include <algorithm>

void TA_MechaGolem::load()
{
    headSprite.load("objects/mecha_golem/head.png", 24, 32);
    headFlashSprite.load("objects/mecha_golem/head.png", 24, 32);
    bodySprite.load("objects/mecha_golem/body.png");
    leftFootSprite.load("objects/mecha_golem/feet.png", 16, 11);
    rightFootSprite.load("objects/mecha_golem/feet.png", 16, 11);
    rightFootSprite.setFrame(1);
    armSprite.load("objects/mecha_golem/arm.png", 16, 16);
    armPartSprite.load("objects/mecha_golem/arm_part.png");

    TA_Camera* camera = objectSet->getLinks().camera;
    headSprite.setCamera(camera);
    headFlashSprite.setCamera(camera);
    bodySprite.setCamera(camera);
    leftFootSprite.setCamera(camera);
    rightFootSprite.setCamera(camera);
    armSprite.setCamera(camera);
    armPartSprite.setCamera(camera);

    hitSound.load("sound/hit.ogg", TA_SOUND_CHANNEL_SFX3);

    position = {double(128 + TA::screenWidth - 61), 112};
    objectSet->getLinks().camera->setLockPosition({128, 0});
    hitboxVector.assign(HITBOX_MAX, HitboxVectorElement());
}

bool TA_MechaGolem::update()
{
    switch(state) {
        case STATE_IDLE:
            updateIdle();
            break;
        case STATE_WAIT:
            updateWait();
            break;
        case STATE_GO_LEFT:
            updateGo(-1);
            break;
        case STATE_GO_RIGHT:
            updateGo(1);
            break;
        case STATE_ARM_MOVE:
            updateArmMove();
            break;
        case STATE_ARM_MOVE_BACK:
            updateArmMoveBack();
            break;
        default:
            break;
    }

    if(state != STATE_ARM_MOVE && state != STATE_ARM_MOVE_BACK && state != STATE_ARM_CIRCLE && state != STATE_ARM_BITE) {
        armPosition = position + TA_Point(-11, -42);
    }

    updateDamage();
    updateHitboxes();
    return true;
}

void TA_MechaGolem::updateIdle()
{
    if(objectSet->getLinks().camera->isLocked()) {
        state = STATE_WAIT;
        objectSet->playBossMusic();
    }
}

void TA_MechaGolem::updateWait()
{
    timer += TA::elapsedTime;
    if(timer < waitTime) {
        return;
    }

    // TODO: check point to line distance here
    double distance = objectSet->getCharacterPosition().getDistance(position + TA_Point(-11, -42));
    if(distance < armMoveMaxDistance) {
        if(TA::random::next() % 3 == 0) {
            initGo();
        }
        else {
            initArmMove();
        }
    }
    else {
        initGo();
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
        state = (TA::random::next() % 4 == 0 ? STATE_GO_LEFT : STATE_GO_RIGHT);
    }
    else {
        state = (TA::random::next() % 4 == 0 ? STATE_GO_RIGHT : STATE_GO_LEFT);
    }
}

void TA_MechaGolem::updateGo(int direction)
{
    timer += TA::elapsedTime;
    if(timer > goTime) {
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

void TA_MechaGolem::updateDamage()
{
    if(invincibleTimer <= invincibleTime) {
        invincibleTimer += TA::elapsedTime;
        return;
    }

    if((objectSet->checkCollision(hitboxVector[HITBOX_WEAK].hitbox) & (TA_COLLISION_EXPLOSION_FIRST | TA_COLLISION_HAMMER)) == 0) {
        return;
    }

    hitSound.play();
    invincibleTimer = 0;
}

void TA_MechaGolem::updateHitboxes()
{
    // TODO: add hitbox to block bombs from back
    hitboxVector[HITBOX_WALL_LEFT].hitbox.setRectangle(TA_Point(112, 0), TA_Point(128, 112));
    hitboxVector[HITBOX_WALL_RIGHT].hitbox.setRectangle(TA_Point(128 + TA::screenWidth, 0), TA_Point(144 + TA::screenWidth, 112));
    hitboxVector[HITBOX_WALL_LEFT].collisionType = hitboxVector[HITBOX_WALL_RIGHT].collisionType = (state == STATE_IDLE ? TA_COLLISION_TRANSPARENT : TA_COLLISION_SOLID);

    hitboxVector[HITBOX_BODY].hitbox.setRectangle(position + TA_Point(5, -44), position + TA_Point(52, -16));
    hitboxVector[HITBOX_WEAK].hitbox.setRectangle(position + TA_Point(8, -55), position + TA_Point(24, -42));
    hitboxVector[HITBOX_BODY].collisionType = hitboxVector[HITBOX_WEAK].collisionType = TA_COLLISION_DAMAGE;

    hitboxVector[HITBOX_ARM].hitbox.setRectangle(armPosition + TA_Point(4, 4), armPosition + TA_Point(12, 12));
    hitboxVector[HITBOX_ARM].collisionType = TA_COLLISION_DAMAGE;
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
    headSprite.draw();
    leftFootSprite.draw();
    rightFootSprite.draw();
    drawArm();

    if(invincibleTimer < damageFlashTime * 4 && int(invincibleTimer / damageFlashTime) % 2 == 0) {
        headFlashSprite.draw();
    }
}

void TA_MechaGolem::drawArm()
{
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
