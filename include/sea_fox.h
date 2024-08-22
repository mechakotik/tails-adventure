#ifndef TA_SEA_FOX_H
#define TA_SEA_FOX_H

#include "pawn.h"
#include "links.h"
#include "sound.h"

class TA_SeaFox : public TA_Pawn {
private:
    enum Item {
        ITEM_VULCAN_GUN = 4
    };

    const double verticalDrag = double(1) / 32;
    const double horizontalDrag = double(1) / 196;
    const double inputDrag = double(1) / 20;
    const double vulcanGunTime = 27;
    const double vulcanGunInterval = 5;
    const double invincibleTime = 120;
    const double gravity = double(1) / 16;

    void physicsStep();
    void updateDirection();
    void updateFollowPosition();
    void updateDrill();
    void updateItem();
    void updateVulcanGun();
    void updateDamage();
    bool checkPawnCollision(TA_Polygon &hitbox) override;

    TA_Links links;
    TA_Point velocity{0, 0};
    TA_Point followPosition;
    TA_Polygon hitbox, drillHitbox;
    TA_Sound bulletSound, damageSound;
    bool flip = false, neededFlip = false;

    double vulcanGunTimer = vulcanGunTime + 1;
    double invincibleTimer = invincibleTime + 1;
    double waterLevel = -64;
    
public:
    void load(TA_Links links);
    void update();
    
    void setSpawnPoint(TA_Point position, bool flip);
    TA_Polygon* getHitbox() {return &hitbox;}
    TA_Polygon* getDrillHitbox() {return &drillHitbox;}
};

#endif // TA_SEA_FOX_H
