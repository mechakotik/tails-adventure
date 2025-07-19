#include "object_set.h"

class TA_HeavyGun : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position, bool flip);
    bool update() override;

private:
    static constexpr float cooldown = 80;

    float timer = 0;
    bool flip = false;
};
