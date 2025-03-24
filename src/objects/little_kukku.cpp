#include "little_kukku.h"
#include "explosion.h"
#include "ring.h"

void TA_LittleKukku::load(TA_Point position) {
    loadFromToml("objects/little_kukku.toml");
    hitbox.setRectangle({3, 2}, {13, 16});
    this->position = position;
    updatePosition();
}

bool TA_LittleKukku::update() {
    switch(state) {
        case State::IDLE:
            updateIdle();
            break;
        case State::COOLDOWN:
            updateCooldown();
            break;
        case State::RUN:
            updateRun();
            break;
    }

    updatePosition();
    setFlip(flip);

    if(objectSet->checkCollision(hitbox) & TA_COLLISION_ATTACK) {
        destroy();
        return false;
    }
    return true;
}

void TA_LittleKukku::updateIdle() {
    setAnimation("idle");
    TA_Point characterDist;
    characterDist.x = objectSet->getCharacterPosition().x - position.x + static_cast<double>(getWidth()) / 2;
    characterDist.y = objectSet->getCharacterPosition().y - position.y + static_cast<double>(getHeight()) / 2;
    if(characterDist.length() < maxCharacterDist && isGoodPosition(position)) {
        state = State::COOLDOWN;
        setAnimation("cooldown");
    }
}

void TA_LittleKukku::updateCooldown() {
    if(!isAnimated()) {
        state = State::RUN;
        flip = (objectSet->getCharacterPosition().x > position.x + static_cast<double>(getWidth()) / 2);
        timer = 0;
    }
}

void TA_LittleKukku::updateRun() {
    setAnimation("run");
    timer += TA::elapsedTime;
    TA_Point newPosition = position;
    newPosition.x += speed * TA::elapsedTime * (flip ? 1 : -1);

    if(!isGoodPosition(newPosition)) {
        flip = !flip;
        newPosition = position;
    }

    position = newPosition;
    if(timer > runTime) {
        state = State::COOLDOWN;
        flip = (objectSet->getCharacterPosition().x > position.x + static_cast<double>(getWidth()) / 2);
        setAnimation("cooldown");
    }
}

bool TA_LittleKukku::isGoodPosition(TA_Point newPosition) {
    constexpr int solidFlags = (TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP | TA_COLLISION_SOLID_DOWN);

    hitbox.setPosition(newPosition);
    bool wall = ((objectSet->checkCollision(hitbox) & solidFlags) != 0);
    hitbox.setPosition(position);
    if(wall) return false;

    TA_Polygon groundHitbox;
    groundHitbox.setRectangle({-4, 16}, {-2, 18});
    groundHitbox.setPosition(newPosition);
    if((objectSet->checkCollision(groundHitbox) & solidFlags) == 0) {
        return false;
    }

    groundHitbox.setRectangle({18, 16}, {20, 18});
    if((objectSet->checkCollision(groundHitbox) & solidFlags) == 0) {
        return false;
    }

    return (objectSet->checkCollision(groundHitbox) & solidFlags) != 0;
}

void TA_LittleKukku::destroy() {
    objectSet->spawnObject<TA_Explosion>(position, 0, TA_EXPLOSION_NEUTRAL);
    objectSet->resetInstaShield();
    if(objectSet->enemyShouldDropRing()) {
        objectSet->spawnObject<TA_Ring>(position + TA_Point(4, 4));
    }
}
