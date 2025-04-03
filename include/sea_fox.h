#ifndef TA_SEA_FOX_H
#define TA_SEA_FOX_H

#include "links.h"
#include "pawn.h"
#include "sound.h"

class TA_SeaFox : public TA_Pawn {
private:
    enum Item { ITEM_VULCAN_GUN = 4, ITEM_ANTI_AIR_MISSILE = 22 };

    const float verticalDrag = float(1) / 32;
    const float horizontalDrag = float(1) / 196;
    const float inputDrag = float(1) / 20;
    const float vulcanGunTime = 27;
    const float vulcanGunInterval = 5;
    const float invincibleTime = 120;
    const float gravity = float(1) / 16;
    const float deadTime = 120;

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
    TA_Point followPosition;
    TA_Rect hitbox, drillHitbox;
    TA_Sound bulletSound, damageSound;
    bool flip = false, neededFlip = false, dead = false;

    float vulcanGunTimer = vulcanGunTime + 1;
    float invincibleTimer = invincibleTime + 1;
    float waterLevel = -64;
    float deadTimer = 0;

public:
    void load(TA_Links links);
    void update();

    void setSpawnPoint(TA_Point position, bool flip);
    void setWaterLevel(float level);
    TA_Rect* getHitbox() { return &hitbox; }
    TA_Rect* getDrillHitbox() { return &drillHitbox; }
    bool gameOver() { return dead && deadTimer >= deadTime; }
};

#endif // TA_SEA_FOX_H
