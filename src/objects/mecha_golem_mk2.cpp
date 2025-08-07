#include "mecha_golem_mk2.h"
#include "sound.h"

void TA_MechaGolemMk2::load(TA_Point position) {
    this->position = position;

    bodySprite.loadFromToml("objects/mecha_golem/mk2_body.toml");
    bodySprite.setCamera(objectSet->getLinks().camera);
    bodySprite.setPosition(position);

    headSprite.loadFromToml("objects/mecha_golem/head.toml");
    headSprite.setCamera(objectSet->getLinks().camera);
    headSprite.setPosition(position + TA_Point(32, 1));
    headSprite.setAnimation("mk2_idle");

    hitboxVector.assign(HITBOX_MAX, HitboxVectorElement());
    hitboxVector[HITBOX_BODY].hitbox.setRectangle({8, 12}, {54, 45});
    hitboxVector[HITBOX_BODY].hitbox.setPosition(position);
    hitboxVector[HITBOX_BODY].collisionType = TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;
    hitboxVector[HITBOX_HEAD].hitbox.setRectangle({1, 2}, {22, 30});
    hitboxVector[HITBOX_HEAD].hitbox.setPosition(position + TA_Point(32, 1));
    hitboxVector[HITBOX_HEAD].collisionType = TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;
}

bool TA_MechaGolemMk2::update() {
    switch(state) {
        case State::WAIT:
            updateWait();
            break;
        case State::IDLE:
            updateIdle();
            break;
    }

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

void TA_MechaGolemMk2::updateIdle() {}

void TA_MechaGolemMk2::draw() {
    bodySprite.draw();
    headSprite.draw();
}
