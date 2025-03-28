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
    const float aimingTime = 30, aimBorder = 32;
    const float flyingTime = 25;
    const float crouchTime = 6;
    const float coolDownTime = 60;
    const float laughTime = 60;
    const float walkSpeed = 0.5, walkDistance = 32, walkBorder = 20;
    const float shortFireDelay = 12, longFireDelay = 20, maxFireAngle = atan(1) * 2 / 3, bulletSpeed = 1;
    const int shortFireBullets = 3, longFireBullets = 10;
    const float invincibleTime = 30;
    const float deathTime = 180, deathExplosionDelay = 6;
    const float jumpWaitTime = 60;
    const float damageFlashTime = 5;

    void updatePosition() override;
    void insertBorderHitboxes();
    void updateDamage();

    TA_Sprite headSprite, bodySprite, feetSprite;
    TA_Sprite headFlashSprite, bodyFlashSprite, feetFlashSprite;

    TA_BirdWalkerState state = TA_BIRD_WALKER_STATE_IDLE;
    TA_Point aimPosition;
    TA_Sound hitSound, jumpSound, fallSound, landSound, shootSound, explosionSound;

    bool flip = false;
    float currentWalkDistance = 0;
    int bulletCounter = 0;

    float invincibleTimeLeft = -1;
    int health = 8;

    std::vector<HitboxVectorElement> borderHitboxVector, defaultHitboxVector, flipHitboxVector;
    TA_Rect weakHitbox;

    float floorY, timer = 0, jumpTimer = 0, flashTimer = damageFlashTime * 5;

public:
    using TA_Object::TA_Object;
    void load(float newFloorY);
    bool update() override;
    void draw() override;
    int getDrawPriority() override { return 1; }
    int getCollisionType() override { return TA_COLLISION_SOLID; }
};

#endif // TA_BIRD_WALKER_H
