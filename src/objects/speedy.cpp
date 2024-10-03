#include "speedy.h"
#include "tools.h"
#include "character.h"
#include "item_box.h"
#include "save.h"
#include "transition.h"
#include "error.h"

void TA_Speedy::load()
{
    double leftX = 256 - static_cast<double>(TA::screenWidth) / 2;
    objectSet->spawnObject<TA_Transition>(TA_Point(leftX, 3888), TA_Point(leftX + 2, 4016), "maps/pm/pm3");

    objectSet->getLinks().camera->setLockPosition({leftX, 112});
    objectSet->getLinks().camera->forceLockX();

    if(isComplete()) {
        objectSet->spawnObject<TA_Transition>(TA_Point(leftX + TA::screenWidth - 2, 3888), TA_Point(leftX + TA::screenWidth, 4016), 3, false);
        return;
    }

    loadSprite();
    initShow();
}

bool TA_Speedy::isComplete()
{
    long long itemMask = TA::save::getSaveParameter("item_mask");
    return itemMask & (1ll << 33);
}

void TA_Speedy::loadSprite()
{
    TA_Sprite::load("objects/speedy.png", 24, 32);
    TA_Sprite::loadAnimationsFromFile("objects/speedy.xml");
    hitbox.setRectangle(TA_Point(6, 10), TA_Point(20, 30));

    characterPlaceholder.load("tails/tails.png", 48, 48);
    characterPlaceholder.loadAnimationsFromFile("tails/animations.xml");
    characterPlaceholder.setCamera(objectSet->getLinks().camera);
}

void TA_Speedy::initShow()
{
    state = STATE_SHOW;
    position = {328, 3883};
    velocity = startShowVelocity;
    setAnimation("show");
    objectSet->getLinks().character->setHide(true);
    characterPlaceholder.setAnimation("look_up");
}

bool TA_Speedy::update()
{
    if(state == STATE_SHOW && isComplete()) {
        return false;
    }

    switch(state) {
        case STATE_SHOW:
            updateShow();
            break;
        case STATE_IDLE:
            updateIdle();
            break;
        case STATE_ATTACK:
            updateAttack();
            break;
        case STATE_FLY_UP_SIMPLE:
            updateFlyUpSimple();
            break;
        case STATE_FLY_UP:
            updateFlyUp();
            break;
        case STATE_END_SEQUENCE:
            updateEndSequence();
            break;
        case STATE_WAIT_ITEM:
            updateWaitItem();
            break;
        case STATE_FLY_AWAY:
            updateFlyAway();
            break;
        case STATE_TRANSITION:
            doTransition();
            return false;
    }

    updatePosition();
    updateFlip();
    return true;
}

void TA_Speedy::updateShow()
{
    if(waiting) {
        timer += TA::elapsedTime;
        if(timer > showWaitTime) {
            waiting = false;
        }
    }
    else {
        velocity.y -= gravity * TA::elapsedTime;
        position = position + velocity * TA::elapsedTime;
    }
    
    characterPlaceholder.setPosition(objectSet->getLinks().character->getPosition());
    characterPlaceholder.setFlip(getDistanceToCharacter().x > 0);

    TA_Point cameraPosition = objectSet->getLinks().camera->getPosition();

    if((position.x < cameraPosition.x - 24 || position.y < cameraPosition.y - 32) && velocity.x < 0) {
        velocity = startShowVelocity;
        velocity.x *= -1;
        waiting = true;
        timer = 0;
    }
    
    else if(position.x > cameraPosition.x + 180 && position.y < cameraPosition.y - 32) {
        objectSet->getLinks().character->setHide(false);
        initIdle();
    }
}

void TA_Speedy::initIdle()
{
    state = STATE_IDLE;
    position = {96, 0};
}

void TA_Speedy::updateIdle()
{
    if(objectSet->getCharacterPosition().y < 3776) {
        TA::sound::playMusic("sound/boss.vgm");
        initAttack();
    }
}

void TA_Speedy::initAttack()
{
    if(objectSet->getLinks().camera->isLocked()) {
        initEndSequence();
        return;
    }
    if(objectSet->getCharacterPosition().y < 384) {
        return;
    }

    state = STATE_ATTACK;
    position.x = objectSet->getCharacterPosition().x - 12;
    position.y = objectSet->getLinks().camera->getPosition().y - 32;
    setAnimation("attack");
}

void TA_Speedy::updateAttack()
{
    position.y += attackSpeed * TA::elapsedTime;
    if(position.y > objectSet->getLinks().camera->getPosition().y + 200) {
        initFlyUp();
    }
}

void TA_Speedy::initFlyUp()
{
    if(objectSet->getCharacterPosition().x > 128) {
        position.x = 192;
    }
    else {
        position.x = 296;
    }
    position.y = objectSet->getLinks().camera->getPosition().y + 176;
    velocity = TA_Point(0, -flyUpSpeed);
    setAnimation("fly_up");

    if(TA::random::next() % 2 == 0) {
        flyUpPhase = 0;
        state = STATE_FLY_UP;
    }
    else {
        state = STATE_FLY_UP_SIMPLE;
    }
}

void TA_Speedy::updateFlyUpSimple()
{
    position = position + velocity * TA::elapsedTime;
    if(position.y < objectSet->getLinks().camera->getPosition().y - 64) {
        initAttack();
    }
}

void TA_Speedy::updateFlyUp()
{
    if(flyUpPhase == 0) {
        double cameraY = objectSet->getLinks().camera->getPosition().y;
        if(position.y < cameraY + 24) {
            flyUpPhase = 1;
            flyUpPhase2Y = cameraY - 8;
        }
    }
    else if(flyUpPhase == 1 && position.y < flyUpPhase2Y) {
        flyUpPhase = 2;
    }

    if(flyUpPhase != 0) {
        if(flyUpPhase == 1) {
            velocity = {0, -flyUpPhase1Speed};
        }
        else {
            velocity = {0, -flyUpPhase2Speed};
        }
        setAnimation("show");
    }

    updateFlyUpSimple();
}

void TA_Speedy::initEndSequence()
{
    state = STATE_END_SEQUENCE;
    position = objectSet->getLinks().camera->getPosition() + TA_Point(240, -34);
    TA::sound::playMusic("sound/pm.vgm");
    endSequencePhase = 0;

    objectSet->getLinks().character->setHide(true);
    cpPosition = objectSet->getLinks().character->getPosition();
    characterPlaceholder.setAnimation("helitail");
    characterPlaceholder.setFlip(false);
}

void TA_Speedy::updateEndSequence()
{
    switch(endSequencePhase) {
        case 0:
            updateEndSequencePhase0();
            break;
        case 1:
            updateEndSequencePhase1();
            break;
        case 2:
            updateEndSequencePhase2();
            break;
        case 3:
            updateEndSequencePhase3();
            break;
        case 4:
            updateEndSequencePhase4();
            break;
        case 5:
            updateEndSequencePhase5();
        default:
            break;
    }

    characterPlaceholder.setPosition(cpPosition);
}

void TA_Speedy::updateEndSequencePhase0()
{
    cpPosition.y -= TA::elapsedTime;
    double needY = objectSet->getLinks().camera->getPosition().y + 48;

    if(cpPosition.y < needY) {
        endSequencePhase = 1;
    }
}

void TA_Speedy::updateEndSequencePhase1()
{
    const double needX = 176;

    if(cpPosition.x < needX) {
        cpPosition.x += TA::elapsedTime;
        cpPosition.x = std::min(cpPosition.x, needX);
    }
    else {
        cpPosition.x -= TA::elapsedTime;
        cpPosition.x = std::max(cpPosition.x, needX);
    }

    if(TA::equal(cpPosition.x, needX)) {
        endSequencePhase = 2;
    }
}

void TA_Speedy::updateEndSequencePhase2()
{
    double needY = objectSet->getLinks().camera->getPosition().y + 89;

    cpPosition.y += TA::elapsedTime;
    if(cpPosition.y > needY) {
        cpPosition.y = needY;
        characterPlaceholder.setAnimation("idle");
        setAnimation("show");
        velocity = {-2, 4};
        endSequencePhase = 3;
    }
}

void TA_Speedy::updateEndSequencePhase3()
{
    velocity.y -= gravity * TA::elapsedTime;
    velocity.y = std::max(velocity.y, double(0));
    position = position + velocity * TA::elapsedTime;

    if(TA::equal(velocity.y, 0)) {
        setAnimation("throw");
        endSequencePhase = 4;
    }
}

void TA_Speedy::updateEndSequencePhase4()
{
    if(!isAnimated()) {
        endSequencePhase = 5;
        velocity = {2, 0};
        setAnimation("show");
        objectSet->spawnObject<TA_ItemBox>(position + TA_Point(4, 16), TA_Point(0, 0), 33, "  chaos  \n emerald ");
        return;
    }

    velocity.x = (getCurrentFrame() == 4 ? 1 : -1);
    velocity.y = 0;
    position = position + velocity * TA::elapsedTime;
}

void TA_Speedy::updateEndSequencePhase5()
{
    velocity.y -= gravity * TA::elapsedTime;
    position = position + velocity * TA::elapsedTime;
    double cameraY = objectSet->getLinks().camera->getPosition().y;

    if(position.x > 388 || position.y < cameraY - 36) {
        endSequencePhase = 6;
        objectSet->getLinks().character->setCharacterPosition(cpPosition);
        objectSet->getLinks().character->setHide(false);
        state = STATE_WAIT_ITEM;
    }
}

void TA_Speedy::updateWaitItem()
{
    if(!(isComplete() && !objectSet->getLinks().character->isGettingItem())) {
        return;
    }

    state = STATE_FLY_AWAY;
    objectSet->getLinks().character->setHide(true);
    cpPosition = objectSet->getLinks().character->getPosition();
    cpVelocity = TA_Point(1, 0);
    characterPlaceholder.setPosition(cpPosition);
    characterPlaceholder.setAnimation("helitail");
    characterPlaceholder.setFlip(false);
}

void TA_Speedy::updateFlyAway()
{
    cpVelocity.y -= flyAwayAcceleration * TA::elapsedTime;
    cpPosition = cpPosition + cpVelocity * TA::elapsedTime;
    characterPlaceholder.setPosition(cpPosition);

    if(cpPosition.y < -48) {
        state = STATE_TRANSITION;
    }
}

void TA_Speedy::updateFlip()
{
    if(state == STATE_FLY_UP && getCurrentFrame() == 7) {
        setFlip(position.x < 256);
    }
    else if(state == STATE_END_SEQUENCE) {
        setFlip(false);
    }
    else {
        setFlip(getAnimationFrame() >= 2);
    }
}

void TA_Speedy::doTransition()
{
    TA::save::setSaveParameter("seafox", true);
    objectSet->setTransition(TA_SCREENSTATE_HOUSE);
}

TA_CollisionType TA_Speedy::getCollisionType()
{
    if(state == STATE_ATTACK) {
        return TA_COLLISION_DAMAGE;
    }
    return TA_COLLISION_TRANSPARENT;
}

void TA_Speedy::draw()
{
    TA_Object::draw();
    if(objectSet->getLinks().character->isHidden()) {
        characterPlaceholder.draw();
    }
}
