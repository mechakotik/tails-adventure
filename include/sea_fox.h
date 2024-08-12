#ifndef TA_SEA_FOX_H
#define TA_SEA_FOX_H

#include "pawn.h"
#include "links.h"

class TA_SeaFox : public TA_Pawn {
private:
    const double verticalDrag = double(1) / 32;
    const double horizontalDrag = double(1) / 196;
    const double inputDrag = double(1) / 20;

    void physicsStep();
    void updateDirection();
    void updateFollowPosition();
    void updateDrill();
    bool checkPawnCollision(TA_Polygon &hitbox) override;

    TA_Links links;
    TA_Point velocity{0, 0};
    TA_Point followPosition;
    TA_Polygon drillHitbox;
    bool flip = false, neededFlip = false;
    
public:
    void load(TA_Links links);
    void update();
    
    TA_Polygon* getDrillHitbox() {return &drillHitbox;}
};

#endif // TA_SEA_FOX_H
