#include "object_set.h"

class TA_UnderwaterGun : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position, bool flip);
    bool update() override;

private:
    static constexpr float cooldown = 120;

    bool flip = false;
    float timer = cooldown + 1;
};

class TA_UnderwaterBullet : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position, bool flip);
    bool update() override;

private:
    static constexpr float speed = 1;

    bool flip = false;
};
