#include <cmath>
#include "bird_walker.h"
#include "tools.h"
#include "error.h"
#include "bullet.h"
#include "explosion.h"
#include "save.h"

void TA_BirdWalker::load(double newFloorY)
{
    if(TA::save::getSaveParameter("boss_mask") & (1ll << 0)) {
        return;
    }

    floorY = newFloorY;
    headSprite.load("objects/bird_walker/head.png", 27, 16);
    headFlashSprite.load("objects/bird_walker/head.png", 27, 16);
    bodySprite.load("objects/bird_walker/body.png", 40, 32);
    bodyFlashSprite.load("objects/bird_walker/body.png", 40, 32);
    feetSprite.load("objects/bird_walker/feet.png", 24, 28);
    feetFlashSprite.load("objects/bird_walker/feet.png", 24, 28);
    aimSprite.load("objects/bird_walker/aim.png", 17, 16);

    jumpSound.load("sound/jump.ogg", TA_SOUND_CHANNEL_SFX2);
    fallSound.load("sound/fall.ogg", TA_SOUND_CHANNEL_SFX2);
    landSound.load("sound/land.ogg", TA_SOUND_CHANNEL_SFX2);
    shootSound.load("sound/shoot.ogg", TA_SOUND_CHANNEL_SFX2);
    hitSound.load("sound/hit.ogg", TA_SOUND_CHANNEL_SFX3);
    explosionSound.load("sound/explosion.ogg", TA_SOUND_CHANNEL_SFX3);

    headSprite.loadAnimationsFromFile("objects/bird_walker/head.xml");
    feetSprite.loadAnimationsFromFile("objects/bird_walker/feet.xml");
    aimSprite.loadAnimationsFromFile("objects/bird_walker/aim.xml");

    headSprite.setCamera(objectSet->getLinks().camera);
    bodySprite.setCamera(objectSet->getLinks().camera);
    feetSprite.setCamera(objectSet->getLinks().camera);
    headFlashSprite.setCamera(objectSet->getLinks().camera);
    bodyFlashSprite.setCamera(objectSet->getLinks().camera);
    feetFlashSprite.setCamera(objectSet->getLinks().camera);
    aimSprite.setCamera(objectSet->getLinks().camera);

    TA_Polygon bodyHitbox;
    bodyHitbox.setRectangle({6, -61}, {33, -29});
    defaultHitboxVector.push_back({bodyHitbox, TA_COLLISION_DAMAGE});
    flipHitboxVector.push_back({bodyHitbox, TA_COLLISION_DAMAGE});

    bodyHitbox.setRectangle({13, -29}, {30, 0});
    defaultHitboxVector.push_back({bodyHitbox, TA_COLLISION_DAMAGE});
    bodyHitbox.setRectangle({9, -29}, {26, 0});
    flipHitboxVector.push_back({bodyHitbox, TA_COLLISION_DAMAGE});

    bodyHitbox.setRectangle({12, -74}, {26, -61});
    defaultHitboxVector.push_back({bodyHitbox, TA_COLLISION_DAMAGE});
    flipHitboxVector.push_back({bodyHitbox, TA_COLLISION_DAMAGE});

    hitbox.setPosition(TA_Point(0, 0));
    hitbox.setRectangle(TA_Point(TA::screenWidth + 576, 0), TA_Point(TA::screenWidth + 592, 448));
    objectSet->getLinks().camera->setLockPosition(TA_Point(576, 64));
}

void TA_BirdWalker::updatePosition()
{
    double offset = (feetSprite.getCurrentFrame() == 4 ? 18 : 27);
    feetSprite.setPosition(position + TA_Point(8, -(feetSprite.getHeight() - 1)));

    offset += bodySprite.getHeight();
    bodySprite.setPosition(position + TA_Point(0, -offset));

    offset += headSprite.getHeight();
    headSprite.setPosition(position + TA_Point((flip ? 5 : 8), -offset));

    headSprite.setFlip(flip);
    bodySprite.setFlip(flip);
    feetSprite.setFlip(flip);

    hitboxVector.clear();
    insertBorderHitboxes();

    for(HitboxVectorElement element : (flip ? flipHitboxVector : defaultHitboxVector)) {
        element.hitbox.setPosition(position + TA_Point(0, feetSprite.getCurrentFrame() == 4 ? 9 : 0));
        if(state == TA_BIRD_WALKER_STATE_DEAD) {
            element.collisionType = TA_COLLISION_TRANSPARENT;
        }
        hitboxVector.push_back(element);
    }

    if(flip) {
        weakHitbox.setRectangle({4, -61}, {8, 0});
    }
    else {
        weakHitbox.setRectangle({31, -61}, {35, 0});
    }
    weakHitbox.setPosition(position);

    headFlashSprite.setPosition(headSprite.getPosition());
    bodyFlashSprite.setPosition(bodySprite.getPosition());
    feetFlashSprite.setPosition(feetSprite.getPosition());
    
    headFlashSprite.setFlip(flip);
    bodyFlashSprite.setFlip(flip);
    feetFlashSprite.setFlip(flip);
}

void TA_BirdWalker::insertBorderHitboxes()
{
    borderHitboxVector.clear();
    TA_Polygon borderHitbox;
    TA_Point cameraPosition = objectSet->getLinks().camera->getPosition();

    borderHitbox.setRectangle(cameraPosition + TA_Point(-16, -16), cameraPosition + TA_Point(0, TA::screenHeight + 16));
    hitboxVector.push_back({borderHitbox, TA_COLLISION_SOLID});

    borderHitbox.setRectangle(cameraPosition + TA_Point(-16, -16), cameraPosition + TA_Point(TA::screenWidth + 16, 0));
    hitboxVector.push_back({borderHitbox, TA_COLLISION_SOLID});
}

bool TA_BirdWalker::update()
{
    if(TA::save::getSaveParameter("boss_mask") & (1ll << 0)) {
        return false;
    }

    auto initAiming = [&] () {
        timer = 0;

        aimPosition.x = objectSet->getCharacterPosition().x - 2;
        aimPosition.x = std::max(aimPosition.x, objectSet->getLinks().camera->getPosition().x + aimBorder);
        aimPosition.x = std::min(aimPosition.x, objectSet->getLinks().camera->getPosition().x + TA::screenWidth - aimBorder);
        aimPosition.y = floorY - 20;

        aimSprite.setPosition(aimPosition);
        aimSprite.setAnimation(flip ? "aiming" : "aiming_flip");
        state = TA_BIRD_WALKER_STATE_AIMING;
    };

    timer += TA::elapsedTime;
    if(flashTimer < damageFlashTime * 4) {
        flashTimer += TA::elapsedTime;
    }

    double centeredX = position.x + bodySprite.getWidth() / 2;
    if(TA::sign(int(centeredX - objectSet->getCharacterPosition().x)) == (flip ? 1 : -1)) {
        jumpTimer += TA::elapsedTime;
    }
    else {
        jumpTimer = 0;
    }

    switch(state) {
        case TA_BIRD_WALKER_STATE_IDLE: {
            if(objectSet->getLinks().camera->isLocked()) {
                updatePosition();
                objectSet->playBossMusic();
                initAiming();
            }
            break;
        }

        case TA_BIRD_WALKER_STATE_AIMING: {
            headSprite.setAnimation("idle");
            if(timer > aimingTime) {
                double centeredX = (aimPosition.x - 12) + bodySprite.getWidth() / 2;
                flip = (TA::sign(int(centeredX - objectSet->getCharacterPosition().x)) < 0);
                timer = 0;
                state = TA_BIRD_WALKER_STATE_LANDING;
                fallSound.play();
            }
            break;
        }

        case TA_BIRD_WALKER_STATE_LANDING: {
            position.x = aimPosition.x - 12;
            position.y = floorY - std::max(double(0), TA::screenHeight * (1 - timer / flyingTime));
            if(timer > flyingTime) {
                timer = 0;
                feetSprite.setFrame(4);
                objectSet->getLinks().camera->shake(24);
                TA::gamepad::rumble(0.75, 0, 20);
                state = TA_BIRD_WALKER_STATE_LANDED;
                landSound.play();
            }
            break;
        }

        case TA_BIRD_WALKER_STATE_LANDED: {
            headSprite.setAnimation("laugh");
            if(timer > crouchTime) {
                feetSprite.setFrame(0);
            }
            if(timer > laughTime) {
                headSprite.setAnimation("idle");
                timer = 0;
                state = TA_BIRD_WALKER_STATE_COOL_DOWN;
            }
            break;
        }

        case TA_BIRD_WALKER_STATE_COOL_DOWN: {
            if(timer > coolDownTime) {
                timer = 0;
                if(jumpTimer < jumpWaitTime) {
                    currentWalkDistance = 0;
                    state = TA_BIRD_WALKER_STATE_WALK;
                }
                else {
                    headSprite.setAnimation("turn");
                    state = TA_BIRD_WALKER_STATE_LAUGH;
                }
            }
            break;
        }

        case TA_BIRD_WALKER_STATE_WALK: {
            feetSprite.setAnimation("walk");
            double centeredPosition = position.x + bodySprite.getWidth() / 2;
            double leftBorder = objectSet->getLinks().camera->getPosition().x + walkBorder;
            double rightBorder = objectSet->getLinks().camera->getPosition().x + TA::screenWidth - walkBorder;

            if((!flip && centeredPosition < leftBorder) || (flip && centeredPosition > rightBorder) || currentWalkDistance > walkDistance) {
                timer = 0;
                feetSprite.setAnimation("idle");
                bulletCounter = 0;
                if((centeredPosition < objectSet->getCharacterPosition().x) == flip
                    && TA::random::next() % 3 == 0) {
                    state = TA_BIRD_WALKER_STATE_FIRE_LONG;
                }
                else {
                    state = TA_BIRD_WALKER_STATE_FIRE_SHORT;
                }
            }
            else {
                position.x += walkSpeed * TA::elapsedTime * (flip ? 1 : -1);
                currentWalkDistance += walkSpeed * TA::elapsedTime;
            }

            break;
        }

        case TA_BIRD_WALKER_STATE_FIRE_SHORT: {
            if(timer > shortFireDelay) {
                if(bulletCounter == shortFireBullets) {
                    state = TA_BIRD_WALKER_STATE_COOL_DOWN;
                }
                else {
                    double angle = double(TA::random::next()) / TA::random::max() * maxFireAngle;
                    TA_Point velocity(bulletSpeed * cos(angle) * (flip ? 1 : -1), bulletSpeed * sin(angle));
                    objectSet->spawnObject<TA_BirdWalkerBullet>(position + TA_Point((flip ? 30  : -6), -64), velocity);
                    bulletCounter ++;
                    shootSound.play();
                }
                timer = 0;
            }
            break;
        }

        case TA_BIRD_WALKER_STATE_FIRE_LONG: {
            feetSprite.setFrame(4);
            if(timer > longFireDelay) {
                if(bulletCounter == longFireBullets) {
                    feetSprite.setFrame(0);
                    state = TA_BIRD_WALKER_STATE_COOL_DOWN;
                }
                else {
                    double angle = double(TA::random::next()) / TA::random::max() * maxFireAngle;
                    TA_Point velocity(bulletSpeed * cos(angle) * (flip ? 1 : -1), bulletSpeed * sin(angle));
                    objectSet->spawnObject<TA_BirdWalkerBullet>(position + TA_Point((flip ? 30  : -6), -55), velocity);
                    bulletCounter ++;
                    shootSound.play();
                }
                timer = 0;
            }
            break;
        }

        case TA_BIRD_WALKER_STATE_LAUGH: {
            if(!headSprite.isAnimated()) {
                headSprite.setAnimation("laugh_flip");
            }
            if(timer > laughTime) {
                headSprite.setAnimation("idle_flip");
                timer = 0;
                state = TA_BIRD_WALKER_STATE_FLYING_UP;
                jumpSound.play();
            }
            break;
        }

        case TA_BIRD_WALKER_STATE_FLYING_UP: {
            if(timer > crouchTime) {
                feetSprite.setFrame(0);
                position.y = floorY - TA::screenHeight * (timer - crouchTime) / flyingTime;
            }
            if(timer > crouchTime + flyingTime) {
                initAiming();
            }
            break;
        }

        case TA_BIRD_WALKER_STATE_DEAD: {
            if(!TA::sound::isPlaying(TA_SOUND_CHANNEL_SFX3)) {
                explosionSound.play();
            }
            headSprite.setAnimation("idle");
            feetSprite.setAnimation("idle");
            int top, bottom = floorY, left = position.x, right = position.x + bodySprite.getWidth();

            switch(int(timer / (deathTime / 6))) {
                case 0:
                    headSprite.setAlpha(196);
                    top = floorY - (feetSprite.getHeight() + bodySprite.getHeight() + headSprite.getHeight());
                    break;

                case 1:
                    headSprite.setAlpha(0);
                    top = floorY - (feetSprite.getHeight() + bodySprite.getHeight());
                    break;

                case 2:
                    bodySprite.setAlpha(196);
                    top = floorY - (feetSprite.getHeight() + bodySprite.getHeight());
                    break;

                case 3:
                    bodySprite.setAlpha(0);
                    top = floorY - feetSprite.getHeight();
                    break;

                case 4:
                    feetSprite.setAlpha(196);
                    top = floorY - feetSprite.getHeight();
                    break;

                default:
                    headSprite.setAlpha(0);
                    bodySprite.setAlpha(0);
                    feetSprite.setAlpha(0);
                    top = floorY;
                    break;
            }

            int previousStep = (timer - TA::elapsedTime) / deathExplosionDelay;
            int currentStep = timer / deathExplosionDelay;
            if(previousStep != currentStep) {
                TA_Point explosionPosition;
                explosionPosition.x = left + TA::random::next() % (right - left + 1) - 8;
                explosionPosition.y = top + TA::random::next() % (bottom - top + 1) - 8;
                objectSet->spawnObject<TA_Explosion>(explosionPosition);
            }

            if(timer > deathTime) {
                objectSet->getLinks().camera->unlock();
                TA::sound::fadeOutChannel(TA_SOUND_CHANNEL_SFX3, 0);
                objectSet->playAreaMusic();

                long long bossMask = TA::save::getSaveParameter("boss_mask");
                bossMask |= (1ll << 0);
                TA::save::setSaveParameter("boss_mask", bossMask);

                return false;
            }
            break;
        }
    }

    updateDamage();
    if(state != TA_BIRD_WALKER_STATE_IDLE) {
        updatePosition();
    }

    headSprite.updateAnimation();
    bodySprite.updateAnimation();
    feetSprite.updateAnimation();

    headFlashSprite.setFrame(headSprite.getCurrentFrame() + 5);
    bodyFlashSprite.setFrame(1);
    feetFlashSprite.setFrame(feetSprite.getCurrentFrame() + 5);

    int flashAlpha = 0;
    if(flashTimer < damageFlashTime * 4) {
        flashAlpha = TA::linearInterpolation(0, 128, flashTimer / damageFlashTime);
    }
    headFlashSprite.setAlpha(flashAlpha);
    bodyFlashSprite.setAlpha(flashAlpha);
    feetFlashSprite.setAlpha(flashAlpha);

    return true;
}

void TA_BirdWalker::updateDamage()
{
    if(invincibleTimeLeft > 0) {
        invincibleTimeLeft -= TA::elapsedTime;
    }
    else if(state != TA_BIRD_WALKER_STATE_AIMING && state != TA_BIRD_WALKER_STATE_FLYING_UP &&
            state != TA_BIRD_WALKER_STATE_LANDING && state != TA_BIRD_WALKER_STATE_DEAD)
    {
        if(objectSet->checkCollision(weakHitbox) & (TA_COLLISION_EXPLOSION_FIRST | TA_COLLISION_INSTA_SHIELD)) {
            health --;
            invincibleTimeLeft = invincibleTime;
            flashTimer = 0;
            hitSound.play();
            if(health <= 0) {
                for(auto &element : defaultHitboxVector) {
                    element.collisionType = TA_COLLISION_TRANSPARENT;
                }
                timer = 0;
                state = TA_BIRD_WALKER_STATE_DEAD;
            }
        }
    }
}

void TA_BirdWalker::draw()
{
    if(state == TA_BIRD_WALKER_STATE_IDLE) {
        return;
    }
    headSprite.draw();
    bodySprite.draw();
    feetSprite.draw();
    if(state == TA_BIRD_WALKER_STATE_AIMING) {
        aimSprite.draw();
    }

    headFlashSprite.draw();
    bodyFlashSprite.draw();
    feetFlashSprite.draw();
}
