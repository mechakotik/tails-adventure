#ifndef TA_LITTLE_KUKKU_H
#define TA_LITTLE_KUKKU_H

#include "object_set.h"

class TA_LittleKukku : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    int getCollisionType() override { return TA_COLLISION_DAMAGE | TA_COLLISION_TARGET; }

private:
    static constexpr double maxCharacterDist = 100;
    static constexpr double speed = 1.33;
    static constexpr double runTime = 90;

    enum class State {
        IDLE,
        COOLDOWN,
        RUN
    };

    void updateIdle();
    void updateCooldown();
    void updateRun();
    bool isGoodPosition(TA_Point newPosition);
    void destroy();

    State state = State::IDLE;
    bool flip = false;
    double timer = 0;
};

#endif // TA_LITTLE_KUKKU_H
