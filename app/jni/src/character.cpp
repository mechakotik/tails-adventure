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
        if(equal(speed.x, 0)) {
            sprite.setAnimation("idle");
        }
        else {
            sprite.setAnimation("walking");
            sprite.setFlip(speed.x < 0);
        }
        if(controller.isPressed(TA_BUTTON_A)) {
            speed.y = jmp * 1.5;
            ground = false;
        }
    }
    else {
        speed.y += grv;
        if(controller.isPressed(TA_BUTTON_A)) {
            speed.y = jmp * 1.5;
            ground = false;
        }
    }

    updateCollisions();
    sprite.setPosition(position);
    updateFollowPosition();
}

void TA_Character::updateCollisions()
{
    TA_Polygon hitbox;
    hitbox.setRectangle(TA_Point(16, 12), TA_Point(27, 39));

    auto checkCollision = [&](TA_Point currentPosition) {
        hitbox.setPosition(currentPosition);
        int mask = 0;
        return links.tilemap->checkCollision(hitbox, 1, mask);
    };

    double left = 0, right = 1;
    while(right - left > 1e-3) {
        double mid = (left + right) / 2;
        if(checkCollision(position + TA_Point(speed.x * mid, 0))) {
            right = mid;
        }
        else {
            left = mid;
        }
    };
    speed.x *= left;
    ground = checkCollision(TA_Point(0, (ground ? 1 : 0.01)));

    hitbox.setRectangle(TA_Point(15, 13), TA_Point(28, 38));
    left = 0, right = 1;
    while(right - left > 1e-3) {
        double mid = (left + right) / 2;
        if(checkCollision(position + TA_Point(0, speed.x * mid))) {
            right = mid;
        }
        else {
            left = mid;
        }
    }
    speed.y *= left;

    position = position + speed;
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
