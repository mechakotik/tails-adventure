#ifndef TA_BIRD_WALKER_H
#define TA_BIRD_WALKER_H

#include <cmath>
#include "object_set.h"

enum TA_BirdWalkerState {
    TA_BIRD_WALKER_STATE_IDLE,
    TA_BIRD_WALKER_STATE_AIMING,
    TA_BIRD_WALKER_STATE_LANDING,
    TA_BIRD_WALKER_STATE_LANDED,
    TA_BIRD_WALKER_STATE_COOL_DOWN,
    TA_BIRD_WALKER_STATE_WALK,
    TA_BIRD_WALKER_STATE_FIRE_SHORT,
    TA_BIRD_WALKER_STATE_FIRE_LONG,
    TA_BIRD_WALKER_STATE_LAUGH,
    TA_BIRD_WALKER_STATE_FLYING_UP,
    TA_BIRD_WALKER_STATE_DEAD
};

class TA_BirdWalker : public TA_Object {
private:
    const double aimingTime = 30, aimBorder = 32;
    const double flyingTime = 25;
    const double crouchTime = 6;
    const double coolDownTime = 60;
    const double laughTime = 60;
    const double walkSpeed = 0.5, walkDistance = 32, walkBorder = 20;
    const double shortFireDelay = 12, longFireDelay = 20, maxFireAngle = atan(1) * 2 / 3, bulletSpeed = 1;
    const int shortFireBullets = 3, longFireBullets = 10;
    const double invincibleTime = 30;
    const double deathTime = 180, deathExplosionDelay = 6;
    const double jumpWaitTime = 60;
    const double damageFlashTime = 5;

    void updatePosition() override;
    void insertBorderHitboxes();
    void updateDamage();

    TA_Sprite headSprite, bodySprite, feetSprite;
    TA_Sprite headFlashSprite, bodyFlashSprite, feetFlashSprite;

    TA_BirdWalkerState state = TA_BIRD_WALKER_STATE_IDLE;
    TA_Point aimPosition;
    TA_Sound hitSound, jumpSound, fallSound, landSound, shootSound, explosionSound;

    bool flip = false;
    double currentWalkDistance = 0;
    int bulletCounter = 0;

    double invincibleTimeLeft = -1;
    int health = 8;

    std::vector<HitboxVectorElement> borderHitboxVector, defaultHitboxVector, flipHitboxVector;
    TA_Polygon weakHitbox;

    double floorY, timer = 0, jumpTimer = 0, flashTimer = damageFlashTime * 5;

public:
    using TA_Object::TA_Object;
    void load(double newFloorY);
    bool update() override;
    void draw() override;
    int getDrawPriority() override { return 1; }
    int getCollisionType() override { return TA_COLLISION_SOLID; }
};

#endif // TA_BIRD_WALKER_H
