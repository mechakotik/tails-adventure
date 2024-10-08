#ifndef TA_BRIDGE_H
#define TA_BRIDGE_H

#include "object_set.h"

enum TA_BridgeState {
    TA_BRIDGE_STATE_IDLE,
    TA_BRIDGE_STATE_DELAY,
    TA_BRIDGE_STATE_FALLING
};

class TA_Bridge : public TA_Object {
private:
    TA_BridgeState state = TA_BRIDGE_STATE_IDLE;
    TA_Sound breakSound;
    TA_Point velocity;
    TA_Polygon collisionHitbox;
    std::string particleFilename;
    double timer = 0;
    bool particlesThrown = false;

    const double delayTime = 25;
    const double fallingTime = 15;
    const double initialSpeed = 0, grv = 0.25;

public:
    using TA_Object::TA_Object;
    void load(TA_Point newPosition, std::string filename, std::string newParticleFilename);
    bool update() override;
    int getCollisionType() override;
};

#endif // TA_BRIDGE_H
