#include "object_set.h"

class TA_RoboKukku : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position, bool flip, bool* lock);
    bool update() override;

private:
    static constexpr float gravity = 0.125;

    TA_Point velocity;
    bool flip = false;
    bool* lock = nullptr;
};

class TA_RoboKukkuSpawner : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position, bool flip);
    bool update() override;

private:
    static constexpr float cooldown = 60;
    static constexpr float globalCooldown = 300;

    float timer = 0;
    float globalTimer = 0;
    bool flip = false;
    bool lock = false;
};
