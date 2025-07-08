#include "object_set.h"

class TA_UnderwaterBarrier : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position, std::string particlePath);
    bool update() override;

private:
    static constexpr float cooldown = 40;
    static constexpr float gravity = 0.125;

    std::string particlePath;
    int hp = 8;
    float timer = cooldown + 1;
    bool passed = false;
};
