#ifndef TA_PILOT_H
#define TA_PILOT_H

#include "object_set.h"

class TA_PilotSpawner : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load() {}
    bool update() override;

private:
    static constexpr float cooldown = 100;

    float timer = 0;
};

class TA_Pilot : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point offset);
    bool update() override;

private:
    static constexpr float gravity = 0.025;
    static constexpr float minYSpeed = -2;

    float ysp = 1;
    float timer = 0;
    bool flip = false;
    bool destroy = false;
};

class TA_DivingPilot : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point offset);
    bool update() override;

private:
    enum class State {
        DIVE,
        TURN_BACK,
        FLY_BACK,
    };

    void updateDive();
    void updateTurnBack();
    void updateFlyBack();

    State state = State::DIVE;
    bool flip = false;
    float timer = 0;
    bool destroy = false;
};

#endif
