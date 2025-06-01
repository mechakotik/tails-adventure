#include "bomber.h"
#include "dead_kukku.h"

void TA_Bomber::load(float aimX, float maxY) {
    loadFromToml("objects/bomber.toml");
    hitbox.setRectangle(TA_Point(2, 2), TA_Point(14, 14));
    this->aimX = aimX;
    this->maxY = maxY;
}

bool TA_Bomber::update() {
    switch(state) {
        case State::IDLE:
            updateIdle();
            break;
        case State::FLY:
            updateFly();
            break;
        case State::PRE_ATTACK:
            updatePreAttack();
            break;
        case State::POST_ATTACK:
            updatePostAttack();
            break;
        case State::FLY_AWAY:
            updateFlyAway();
            break;
    }

    updatePosition();
    if(state != State::IDLE && (objectSet->checkCollision(hitbox) & TA_COLLISION_ATTACK) != 0) {
        objectSet->spawnObject<TA_DeadKukku>(position);
        return false;
    }
    return true;
}

void TA_Bomber::updateIdle() {
    float characterX = objectSet->getCharacterPosition().x;
    float startY = objectSet->getLinks().camera->getPosition().y - static_cast<float>(getHeight()) - 1;
    if(lastCharacterX < aimX && aimX <= characterX && startY + heightShift <= maxY) {
        position = {aimX + 64, startY};
        velocity = {-1, startYSpeed};
        setAnimation("fly");
        state = State::FLY;
    }
    lastCharacterX = characterX;
}

void TA_Bomber::updateFly() {
    velocity.y -= gravity * TA::elapsedTime;
    if(velocity.y < 0) {
        timer = 0;
        state = State::PRE_ATTACK;
    } else {
        position += velocity * TA::elapsedTime;
    }
}

void TA_Bomber::updatePreAttack() {
    timer += TA::elapsedTime;
    if(timer > attackTime) {
        timer = 0;
        state = State::POST_ATTACK;
    }
}

void TA_Bomber::updatePostAttack() {
    timer += TA::elapsedTime;
    if(timer > attackTime) {
        timer = 0;
        state = State::FLY_AWAY;
    }
}

void TA_Bomber::updateFlyAway() {
    velocity.y -= gravity * TA::elapsedTime;
    position += velocity * TA::elapsedTime;
    if(position.y < objectSet->getLinks().camera->getPosition().y - static_cast<float>(getHeight()) - 1) {
        state = State::IDLE;
    }
}

void TA_Bomber::draw() {
    if(state != State::IDLE) {
        TA_Sprite::draw();
    }
}

int TA_Bomber::getCollisionType() {
    if(state == State::IDLE) {
        return TA_COLLISION_TRANSPARENT;
    }
    return TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;
}
