#ifndef TA_SEA_FOX_H
#define TA_SEA_FOX_H

#include "links.h"
#include "pawn.h"
#include "sound.h"

class TA_SeaFox : public TA_Pawn {
private:
    enum Item { ITEM_VULCAN_GUN = 4, ITEM_EXTRA_SPEED = 20, ITEM_ANTI_AIR_MISSILE = 22 };

    static constexpr float verticalDrag = 1.0F / 32;
    static constexpr float horizontalDrag = 1.0F / 196;
    static constexpr float inputDrag = 1.0F / 20;
    static constexpr float vulcanGunTime = 27;
    static constexpr float vulcanGunInterval = 5;
    static constexpr float invincibleTime = 120;
    static constexpr float deadGravity = 1.0F / 16;
    static constexpr float deadTime = 120;

    static constexpr float gravity = 0.125;
    static constexpr float initJumpSpeed = -3.75;
    static constexpr float minJumpSpeed = -2.5;
    static constexpr float maxYSpeed = 3;
    static constexpr float releaseJumpSpeed = -1.5;
    static constexpr float extraSpeedTime = 140;
    static constexpr float extraSpeedAddTime = 40;
    static constexpr float extraSpeedAddYSpeed = -3.25;

    void physicsStep();
    void updateDirection();
    void updateFollowPosition();
    void updateDrill();
    void updateItem();
    void updateVulcanGun();
    void updateDamage();
    void dropRings();
    void updateDead();
    bool checkPawnCollision(TA_Rect& hitbox) override;

    TA_Links links;
    TA_Point velocity{0, 0};
    TA_Point velocityAdd{0, 0};
    TA_Point followPosition;
    TA_Rect hitbox, drillHitbox;
    TA_Sound bulletSound, damageSound, jumpSound, extraSpeedSound;
    bool flip = false, neededFlip = false, dead = false;

    float vulcanGunTimer = vulcanGunTime + 1;
    float invincibleTimer = invincibleTime + 1;
    float waterLevel = -64;
    float deadTimer = 0;

    bool groundMode = false;
    bool ground = false;
    float jumpSpeed = 0;
    bool jump = false;
    bool jumpReleased = false;
    bool extraSpeed = false;
    float extraSpeedReleaseTime = 140;
    float extraSpeedTimer = extraSpeedReleaseTime + extraSpeedAddTime + 1;

public:
    void load(TA_Links links);
    void update();

    void setSpawnPoint(TA_Point position, bool flip);
    void setGroundMode() { groundMode = true; }
    void setWaterLevel(float level) { waterLevel = level; }
    void setVelocityAdd(TA_Point add) { velocityAdd = add; }
    TA_Rect* getHitbox() { return &hitbox; }
    TA_Rect* getDrillHitbox() { return &drillHitbox; }
    bool gameOver() { return dead && deadTimer >= deadTime; }
};

#endif // TA_SEA_FOX_H
