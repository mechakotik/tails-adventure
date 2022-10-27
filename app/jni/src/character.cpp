#include <algorithm>
#include "character.h"
#include "globals.h"
#include "tools.h"
#include "error.h"

void TA_Character::load(TA_GameScreenLinks newLinks)
{
    links = newLinks;
    position = {0, 128};
    updateFollowPosition();
    links.camera->setFollowPosition(&followPosition);
    controller.load();

    sprite.load("tails/tails.png", 48, 48);
    sprite.loadAnimationsFromFile("tails/animations.xml");
    sprite.setCamera(links.camera);
    speed.y = jmp;
}

void TA_Character::update()
{
    controller.update();
    verticalMove();

    if(ground) {
        speed.y = 0;
        if(equal(speed.x, 0)) {
            sprite.setAnimation("idle");
        }
        else {
            sprite.setAnimation("walking");
            sprite.setFlip(speed.x < 0);
        }
    }
    else {
        speed.y += grv;
    }

    updateCollisions();
    sprite.setPosition(position);
    updateFollowPosition();
}

void TA_Character::updateCollisions()
{
    hitbox.setPosition(position);
    hitbox.setRectangle(TA_Point(15, 12), TA_Point(28, 39));
    double minVectorPos = 1;
    int flags = 0;
    links.tilemap->updateCollisions(hitbox, speed, 1, minVectorPos, flags);
    speed = speed * TA_Point(minVectorPos, minVectorPos);
    position = position + speed;
    if(flags != 0 && !ground) {
        printLog("%f", minVectorPos);
        ground = true;
    }

    position.x = std::max(position.x, double(0));
    position.x = std::min(position.x, double(links.tilemap->getWidth()));
    position.y = std::max(position.y, double(0));
    position.y = std::min(position.y, double(links.tilemap->getHeight()));
}

void TA_Character::verticalMove()
{
    TA_Direction direction = controller.getDirection();

    if(direction == TA_DIRECTION_RIGHT) {
        speed.x += acc;
        speed.x = std::min(speed.x, top);
    }
    else if(direction == TA_DIRECTION_LEFT) {
        speed.x -= acc;
        speed.x = std::max(speed.x, -top);
    }
    else {
        if(speed.x > 0) {
            speed.x = std::max(double(0), speed.x - acc);
        }
        else {
            speed.x = std::min(double(0), speed.x + acc);
        }
    }
}

void TA_Character::updateFollowPosition()
{
    followPosition = position + TA_Point(22 - gScreenWidth / 2, 26 - gScreenHeight / 2);
}
