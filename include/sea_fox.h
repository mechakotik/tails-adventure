#ifndef TA_SEA_FOX_H
#define TA_SEA_FOX_H

#include "pawn.h"
#include "links.h"

class TA_SeaFox : public TA_Pawn {
private:
    enum Item {
        ITEM_VULCAN_GUN = 4
    };

    const double verticalDrag = double(1) / 32;
    const double horizontalDrag = double(1) / 196;
    const double inputDrag = double(1) / 20;
    const double vulcanGunTime = 40;
    const double vulcanGunInterval = 6;

    void physicsStep();
    void updateDirection();
    void updateFollowPosition();
    void updateDrill();
    void updateItem();
    void updateVulcanGun();
    bool checkPawnCollision(TA_Polygon &hitbox) override;

    TA_Links links;
    TA_Point velocity{0, 0};
    TA_Point followPosition;
    TA_Polygon drillHitbox;
    bool flip = false, neededFlip = false;

    double vulcanGunTimer = vulcanGunTime + 1;
    
public:
    void load(TA_Links links);
    void update();
    
    TA_Polygon* getDrillHitbox() {return &drillHitbox;}
};

#endif // TA_SEA_FOX_H
