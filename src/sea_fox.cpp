#include "sea_fox.h"
#include "controller.h"
#include "object_set.h"
#include "bullet.h"
#include "hud.h"
#include "error.h"

void TA_SeaFox::load(TA_Links links)
{
    this->links = links;

    TA_Sprite::load("tails/seafox.png", 32, 32);
    TA_Sprite::loadAnimationsFromFile("tails/seafox.xml");
    TA_Sprite::setCamera(links.camera);
    TA_Sprite::setAnimation("idle");

    bulletSound.load("sound/bullet.ogg", TA_SOUND_CHANNEL_SFX3);
    damageSound.load("sound/damage.ogg", TA_SOUND_CHANNEL_SFX1);

    hitbox.setRectangle(TA_Point(9, 4), TA_Point(23, 30));

    if(TA::levelPath=="maps/lr/lr1") { // TODO: set water level in area XML
        waterLevel = 96;
    }
}

void TA_SeaFox::setSpawnPoint(TA_Point position, bool flip)
{
    this->position = position;
    this->flip = this->neededFlip = flip;
    updateFollowPosition();
    links.camera->setFollowPosition(&followPosition);
}

void TA_SeaFox::update()
{
    physicsStep();

    updateDirection();
    updateFollowPosition();
    updateDrill();
    updateItem();
    updateDamage();

    if(invincibleTimer < invincibleTime) {
        setAlpha(200);
    }
    else {
        setAlpha(255);
    }

    setFlip(flip);
    //TA::printLog("%f %f", position.x, position.y);
}

void TA_SeaFox::physicsStep()
{
    auto updateSpeed = [] (double &currentSpeed, double neededSpeed, double drag)
    {
        if(currentSpeed > neededSpeed) {
            currentSpeed = std::max(neededSpeed, currentSpeed - drag * TA::elapsedTime);
        }
        else {
            currentSpeed = std::min(neededSpeed, currentSpeed + drag * TA::elapsedTime);
        }
    };

    bool underwater = (position.y + 30 > waterLevel);

    if(links.controller->getDirection() != TA_DIRECTION_MAX) {
        TA_Point vector = links.controller->getDirectionVector();
        updateSpeed(velocity.x, vector.x, inputDrag);
        if(underwater) {
            updateSpeed(velocity.y, vector.y, inputDrag);
        }
    }
    else {
        updateSpeed(velocity.x, 0, horizontalDrag);
        if(underwater) {
            updateSpeed(velocity.y, 0, verticalDrag);
        }
    }

    if(!underwater) {
        velocity.y = std::min(double(1), velocity.y + gravity * TA::elapsedTime);
    }

    moveAndCollide(TA_Point(9, 4), TA_Point(23, 30), velocity * TA::elapsedTime, false);
    TA_Sprite::setPosition(position);
}

bool TA_SeaFox::checkPawnCollision(TA_Polygon &hitbox)
{
    int flags = links.objectSet->checkCollision(hitbox);
    if(flags & TA_COLLISION_SOLID) {
        return true;
    }
    return false;
}

void TA_SeaFox::updateDirection()
{
    if(flip != neededFlip) {
        if(!isAnimated()) {
            setAnimation("idle");
            flip = neededFlip;
        }
    }
    else if(links.controller->isJustPressed(TA_BUTTON_A)) {
        setAnimation("rotate");
        neededFlip = !flip;
    }
}

void TA_SeaFox::updateFollowPosition()
{
    followPosition = position + TA_Point(getWidth() / 2, getHeight() / 2) - TA_Point(TA::screenWidth / 2, TA::screenHeight / 2);
    followPosition.x += (flip ? -1 : 1) * (TA::screenWidth * 0.15);
}

void TA_SeaFox::updateDrill()
{
    if(flip) {
        drillHitbox.setRectangle(TA_Point(2, 10), TA_Point(16, 24));
    }
    else {
        drillHitbox.setRectangle(TA_Point(24, 10), TA_Point(30, 24));
    }
    drillHitbox.setPosition(position);
}

void TA_SeaFox::updateItem()
{
    if(vulcanGunTimer < vulcanGunTime) {
        updateVulcanGun();
        return;
    }
    if(!links.controller->isJustPressed(TA_BUTTON_B)) {
        return;
    }

    switch(links.hud->getCurrentItem()) {
        case ITEM_VULCAN_GUN:
            vulcanGunTimer = 0;
            break;
        default:
            damageSound.play();
            break;
    }
}

void TA_SeaFox::updateVulcanGun()
{
    int prev = vulcanGunTimer / vulcanGunInterval;
    vulcanGunTimer += TA::elapsedTime;
    int cur = vulcanGunTimer / vulcanGunInterval;

    if(prev != cur) {
        TA_Point bulletPosition = position + TA_Point((flip ? 0 : 26), 20);
        TA_Point bulletVelocity = TA_Point((flip ? -5 : 5), 0);
        links.objectSet->spawnObject<TA_VulcanGunBullet>(bulletPosition, bulletVelocity);
        bulletSound.play();
    }
}

void TA_SeaFox::updateDamage()
{
    if(invincibleTimer < invincibleTime) {
        invincibleTimer += TA::elapsedTime;
        return;
    }

    hitbox.setPosition(position);
    if(links.objectSet->checkCollision(hitbox) & TA_COLLISION_DAMAGE) {
        links.objectSet->addRings(-2);
        velocity = velocity * -1;
        invincibleTimer = 0;
        damageSound.play();
    }
}
