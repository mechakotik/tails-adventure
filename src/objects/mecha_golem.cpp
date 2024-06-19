#include "mecha_golem.h"

void TA_MechaGolem::load()
{
    headSprite.load("objects/mecha_golem/head.png", 24, 32);
    headFlashSprite.load("objects/mecha_golem/head.png", 24, 32);
    bodySprite.load("objects/mecha_golem/body.png");
    leftFootSprite.load("objects/mecha_golem/feet.png", 16, 11);
    rightFootSprite.load("objects/mecha_golem/feet.png", 16, 11);
    rightFootSprite.setFrame(1);

    headSprite.setCamera(objectSet->getLinks().camera);
    headFlashSprite.setCamera(objectSet->getLinks().camera);
    bodySprite.setCamera(objectSet->getLinks().camera);
    leftFootSprite.setCamera(objectSet->getLinks().camera);
    rightFootSprite.setCamera(objectSet->getLinks().camera);

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
        case STATE_P1_WAIT:
            updateP1Wait();
            break;
    }

    updateDamage();
    updateHitboxes();
    return true;
}

void TA_MechaGolem::updateIdle()
{
    if(objectSet->getLinks().camera->isLocked()) {
        state = STATE_P1_WAIT;
        objectSet->playBossMusic();
    }
}

void TA_MechaGolem::updateP1Wait()
{

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
    hitboxVector[HITBOX_WALL_LEFT].hitbox.setRectangle(TA_Point(112, 0), TA_Point(128, 112));
    hitboxVector[HITBOX_WALL_RIGHT].hitbox.setRectangle(TA_Point(128 + TA::screenWidth, 0), TA_Point(144 + TA::screenWidth, 112));
    hitboxVector[HITBOX_WALL_LEFT].collisionType = hitboxVector[HITBOX_WALL_RIGHT].collisionType = (state == STATE_IDLE ? TA_COLLISION_TRANSPARENT : TA_COLLISION_SOLID);

    hitboxVector[HITBOX_BODY].hitbox.setRectangle(position + TA_Point(5, -44), position + TA_Point(52, -16));
    hitboxVector[HITBOX_WEAK].hitbox.setRectangle(position + TA_Point(8, -55), position + TA_Point(24, -42));
    hitboxVector[HITBOX_BODY].collisionType = hitboxVector[HITBOX_WEAK].collisionType = TA_COLLISION_DAMAGE;
}

void TA_MechaGolem::draw()
{
    headSprite.setPosition(position + TA_Point(5, -56));
    bodySprite.setPosition(position + TA_Point(0, -57));
    leftFootSprite.setPosition(position + TA_Point(5, -10));
    rightFootSprite.setPosition(position + TA_Point(26, -10));

    headFlashSprite.setPosition(headSprite.getPosition());
    headFlashSprite.setFrame(headSprite.getCurrentFrame() + 5);

    bodySprite.draw();
    headSprite.draw();
    leftFootSprite.draw();
    rightFootSprite.draw();

    if(invincibleTimer < damageFlashTime * 4 && int(invincibleTimer / damageFlashTime) % 2 == 0) {
        headFlashSprite.draw();
    }
}
