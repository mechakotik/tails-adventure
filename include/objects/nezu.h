#include "object_set.h"

class TA_Nezu : public TA_Object {
private:
    const double speed = 0.5;
    const double placeTime = 30;
    const double attackTime = 60;

    TA_Point getDistanceToCharacter();
    bool isBadPosition(TA_Point position);
    bool isCloseToCharacter();

    void updateIdle();
    void updateWalk();
    void updateAttack();

    enum State {
        STATE_IDLE,
        STATE_WALK,
        STATE_ATTACK
    };

    State state = STATE_IDLE;
    bool direction = 0, bombPlaced = 0;
    double timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    TA_CollisionType getCollisionType() {return TA_COLLISION_DAMAGE;}
};
