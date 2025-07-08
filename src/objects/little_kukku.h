#ifndef TA_LITTLE_KUKKU_H
#define TA_LITTLE_KUKKU_H

#include "object_set.h"

class TA_LittleKukku : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;

private:
    static constexpr float maxCharacterDist = 100;
    static constexpr float speed = 1.33;
    static constexpr float runTime = 90;

    enum class State { IDLE, COOLDOWN, RUN };

    void updateIdle();
    void updateCooldown();
    void updateRun();
    bool isGoodPosition(TA_Point newPosition);
    void destroy() override;

    State state = State::IDLE;
    bool flip = false;
    float timer = 0;
};

#endif // TA_LITTLE_KUKKU_H
