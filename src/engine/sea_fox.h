#ifndef TA_SEA_FOX_H
#define TA_SEA_FOX_H

#include "geometry.h"
#include "links.h"
#include "sound.h"
#include "sprite.h"

class TA_SeaFox : public TA_Sprite {
private:
    enum Item : uint8_t {
        ITEM_VULCAN_GUN = 4,
        ITEM_EXTRA_SPEED = 20,
        ITEM_EXTRA_ARMOR = 21,
        ITEM_ANTI_AIR_MISSILE = 22,
        ITEM_MINE = 24
    };

    static constexpr float verticalDrag = 1.0F / 32;
    static constexpr float horizontalDrag = 1.0F / 196;
    static constexpr float inputDrag = 1.0F / 20;
    static constexpr float airDrag = 1.0F / 12;
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
    static constexpr float sparklePeriod = 6;
    static constexpr float flyXSpeed = 2;
    static constexpr float flyYSpeed = 1.5;

    void physicsStep();
    void updateDirection();
    void updateFollowPosition();
    void updateDrill();
    void updateItem();
    void updateVulcanGun();
    void updateSparkle();
    void updateDamage();
    void dropRings();
    void updateDead();

    TA_Links links;
    TA_Point position;
    TA_Point velocity{0, 0};
    TA_Point velocityAdd{0, 0};
    TA_Point followPosition;
    TA_Rect hitbox, drillHitbox;
    TA_Sound bulletSound, damageSound, jumpSound, extraSpeedSound, waterSound;
    bool flip = false, neededFlip = false, dead = false;

    float vulcanGunTimer = vulcanGunTime + 1;
    float invincibleTimer = invincibleTime + 1;
    float deadTimer = 0;

    bool groundMode = false;
    bool flyMode = false;

    bool ground = false;
    float jumpSpeed = 0;
    bool jump = false;
    bool jumpReleased = false;
    bool extraSpeed = false;
    float extraSpeedReleaseTime = 140;
    float extraSpeedTimer = extraSpeedReleaseTime + extraSpeedAddTime + 1;

    bool extraArmor = false;
    float sparkleTimer = 0;

    bool hidden = false;

    bool debugMode = false;
    bool noclip = false;

public:
    void load(TA_Links links);
    void update();

    void setSpawnPoint(TA_Point position, bool flip);
    void setGroundMode() { groundMode = true; }
    void setFlyMode() { flyMode = true; }
    void setVelocityAdd(TA_Point add) { velocityAdd = add; }
    void setHide(bool hidden);
    TA_Rect* getHitbox() { return &hitbox; }
    TA_Rect* getDrillHitbox() { return &drillHitbox; }
    bool isFastCamera() { return debugMode; }
    bool gameOver() { return dead && deadTimer >= deadTime; }
};

#endif // TA_SEA_FOX_H
