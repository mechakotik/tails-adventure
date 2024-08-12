#ifndef TA_SEA_FOX_H
#define TA_SEA_FOX_H

#include "pawn.h"
#include "links.h"

class TA_SeaFox : public TA_Pawn {
private:
    const double verticalDrag = double(1) / 32;
    const double horizontalDrag = double(1) / 196;
    const double inputDrag = double(1) / 24;

    void physicsStep();
    void updateDirection();
    void updateFollowPosition();
    bool checkPawnCollision(TA_Polygon &hitbox) override;

    TA_Links links;
    TA_Point velocity{0, 0};
    TA_Point followPosition;
    bool flip = false, neededFlip = false;
    
public:
    void load(TA_Links links);
    void update();
};

#endif // TA_SEA_FOX_H
