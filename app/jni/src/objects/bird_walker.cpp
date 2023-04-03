#include "bird_walker.h"
#include "engine/tools.h"
#include "engine/error.h"

void TA_BirdWalker::load(double newFloorY)
{
    floorY = newFloorY;
    headSprite.load("objects/bird_walker/head.png", 27, 16);
    bodySprite.load("objects/bird_walker/body.png");
    feetSprite.load("objects/bird_walker/feet.png", 24, 28);
    aimSprite.load("objects/bird_walker/aim.png", 17, 16);
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
    for(HitboxVectorElement element : borderHitboxVector) {
        hitboxVector.push_back(element);
    }

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

bool TA_BirdWalker::update()
{
    auto initAiming = [&] () {
        timer = 0;
        flip = TA::random::next() % 2;

        aimPosition.x = objectSet->getCharacterPosition().x - 2;
        aimPosition.x = std::max(aimPosition.x, objectSet->getCamera()->getPosition().x + aimBorder);
        aimPosition.x = std::min(aimPosition.x, objectSet->getCamera()->getPosition().x + TA::screenWidth - aimBorder);
        aimPosition.y = floorY - 20;

        aimSprite.setPosition(aimPosition);
        aimSprite.setAnimation(flip ? "aiming" : "aiming_flip");
        state = TA_BIRD_WALKER_STATE_AIMING;
    };

    switch(state) {
        case TA_BIRD_WALKER_STATE_IDLE: {
            if(objectSet->getCamera()->isLocked()) { // TODO: boss music and SFX
                TA_Polygon borderHitbox;
                TA_Point cameraPosition = objectSet->getCamera()->getPosition();

                borderHitbox.setRectangle(cameraPosition + TA_Point(-1, 0), cameraPosition + TA_Point(0, TA::screenHeight));
                borderHitboxVector.push_back({borderHitbox, TA_COLLISION_SOLID});

                borderHitbox.setRectangle(cameraPosition + TA_Point(0, -1), cameraPosition + TA_Point(TA::screenWidth, 0));
                borderHitboxVector.push_back({borderHitbox, TA_COLLISION_SOLID});

                borderHitbox.setRectangle(cameraPosition + TA_Point(TA::screenWidth, 0), cameraPosition + TA_Point(TA::screenWidth + 1, TA::screenHeight));
                borderHitboxVector.push_back({borderHitbox, TA_COLLISION_SOLID});
                initAiming();
            }

            break;
        }

        case TA_BIRD_WALKER_STATE_AIMING: {
            timer += TA::elapsedTime;
            if(timer > aimingTime) {
                timer = 0;
                state = TA_BIRD_WALKER_STATE_LANDING;
            }
            break;
        }

        case TA_BIRD_WALKER_STATE_LANDING: {
            timer += TA::elapsedTime;
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
            timer += TA::elapsedTime;
            if(timer > crouchTime) {
                feetSprite.setFrame(0);
                timer = 0;
                state = TA_BIRD_WALKER_STATE_COOL_DOWN;
            }
            break;
        }

        case TA_BIRD_WALKER_STATE_COOL_DOWN: {
            timer += TA::elapsedTime;
            if(timer > coolDownTime) {
                timer = 0;
                state = TA_BIRD_WALKER_STATE_FLYING_UP;
            }
            break;
        }

        case TA_BIRD_WALKER_STATE_FLYING_UP: {
            timer += TA::elapsedTime;
            if(timer > crouchTime) {
                feetSprite.setFrame(0);
                position.y = floorY - TA::screenHeight * (timer - crouchTime) / flyingTime;
            }
            if(timer > crouchTime + flyingTime) {
                initAiming();
            }
            break;
        }
    }

    if(state != TA_BIRD_WALKER_STATE_IDLE) {
        updatePosition();
    }
    return true;
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
