#include "bird_walker.h"
#include "tools.h"
#include "error.h"
#include "bullet.h"
#include "explosion.h"

void TA_BirdWalker::load(double newFloorY)
{
    floorY = newFloorY;
    headSprite.load("objects/bird_walker/head.png", 27, 16);
    bodySprite.load("objects/bird_walker/body.png");
    feetSprite.load("objects/bird_walker/feet.png", 24, 28);
    aimSprite.load("objects/bird_walker/aim.png", 17, 16);

    headSprite.loadAnimationsFromFile("objects/bird_walker/head.xml");
    feetSprite.loadAnimationsFromFile("objects/bird_walker/feet.xml");
    aimSprite.loadAnimationsFromFile("objects/bird_walker/aim.xml");

    headSprite.setCamera(objectSet->getCamera());
    bodySprite.setCamera(objectSet->getCamera());
    feetSprite.setCamera(objectSet->getCamera());
    aimSprite.setCamera(objectSet->getCamera());

    TA_Polygon bodyHitbox; // TODO: crouch hitbox, flip hitbox
    bodyHitbox.setRectangle({6, -61}, {33, -29});
    defaultHitboxVector.push_back({bodyHitbox, TA_COLLISION_DAMAGE});

    bodyHitbox.setRectangle({13, -29}, {30, 0});
    defaultHitboxVector.push_back({bodyHitbox, TA_COLLISION_DAMAGE});

    bodyHitbox.setRectangle({12, -74}, {26, -61});
    defaultHitboxVector.push_back({bodyHitbox, TA_COLLISION_DAMAGE});
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

    if(feetSprite.getCurrentFrame() == 4) {
        for(HitboxVectorElement element : crouchHitboxVector) {
            element.hitbox.setPosition(position);
            hitboxVector.push_back(element);
        }
    }
    else {
        for(HitboxVectorElement element : defaultHitboxVector) {
            element.hitbox.setPosition(position);
            hitboxVector.push_back(element);
        }
    }
}

void TA_BirdWalker::insertBorderHitboxes()
{
    borderHitboxVector.clear();
    TA_Polygon borderHitbox;
    TA_Point cameraPosition = objectSet->getCamera()->getPosition();

    borderHitbox.setRectangle(cameraPosition + TA_Point(-1, 0), cameraPosition + TA_Point(0, TA::screenHeight));
    hitboxVector.push_back({borderHitbox, TA_COLLISION_SOLID});

    borderHitbox.setRectangle(cameraPosition + TA_Point(0, -1), cameraPosition + TA_Point(TA::screenWidth, 0));
    hitboxVector.push_back({borderHitbox, TA_COLLISION_SOLID});

    borderHitbox.setRectangle(cameraPosition + TA_Point(TA::screenWidth, 0), cameraPosition + TA_Point(TA::screenWidth + 1, TA::screenHeight));
    hitboxVector.push_back({borderHitbox, TA_COLLISION_SOLID});
}

bool TA_BirdWalker::update()
{
    auto initAiming = [&] () {
        timer = 0;

        aimPosition.x = objectSet->getCharacterPosition().x - 2;
        aimPosition.x = std::max(aimPosition.x, objectSet->getCamera()->getPosition().x + aimBorder);
        aimPosition.x = std::min(aimPosition.x, objectSet->getCamera()->getPosition().x + TA::screenWidth - aimBorder);
        aimPosition.y = floorY - 20;

        aimSprite.setPosition(aimPosition);
        aimSprite.setAnimation(flip ? "aiming" : "aiming_flip");
        state = TA_BIRD_WALKER_STATE_AIMING;
    };

    timer += TA::elapsedTime;

    switch(state) {
        case TA_BIRD_WALKER_STATE_IDLE: {
            if(objectSet->getCamera()->isLocked()) { // TODO: boss music and SFX
                updatePosition();
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
            }
            break;
        }

        case TA_BIRD_WALKER_STATE_LANDING: {
            position.x = aimPosition.x - 12;
            position.y = floorY - std::max(double(0), TA::screenHeight * (1 - timer / flyingTime));
            if(timer > flyingTime) {
                timer = 0;
                feetSprite.setFrame(4);
                objectSet->getCamera()->shake(24);
                state = TA_BIRD_WALKER_STATE_LANDED;
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
                double centeredX = position.x + bodySprite.getWidth() / 2;
                if(TA::sign(int(centeredX - objectSet->getCharacterPosition().x)) == (flip ? -1 : 1)) {
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
            double leftBorder = objectSet->getCamera()->getPosition().x + walkBorder;
            double rightBorder = objectSet->getCamera()->getPosition().x + TA::screenWidth - walkBorder;

            if((!flip && centeredPosition < leftBorder) || (flip && centeredPosition > rightBorder) || currentWalkDistance > walkDistance) {
                timer = 0;
                feetSprite.setAnimation("idle");
                bulletCounter = 0;
                if(TA::random::next() % 3 == 0) {
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
                objectSet->getCamera()->unlock();
                return false;
            }
            break;
        }
    }

    updateDamage();
    if(state != TA_BIRD_WALKER_STATE_IDLE) {
        updatePosition();
    }
    return true;
}

void TA_BirdWalker::updateDamage()
{
    if(invincibleTimeLeft > 0) {
        invincibleTimeLeft -= TA::elapsedTime;
    }
    else if(state != TA_BIRD_WALKER_STATE_AIMING && state != TA_BIRD_WALKER_STATE_FLYING_UP && state != TA_BIRD_WALKER_STATE_LANDING) {
        bool damaged = false;
        for(int pos = 3; pos < (int)hitboxVector.size(); pos ++) {
            if(objectSet->checkCollision(hitboxVector[pos].hitbox) & TA_COLLISION_EXPLOSION) {
                damaged = true;
                break;
            }
        }
        if(damaged) {
            health --;
            invincibleTimeLeft = invincibleTime;
            if(health <= 0) {
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
}
