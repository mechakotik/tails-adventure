#include "object_set.h"

class TA_Mine : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position, float xsp);
    bool update() override;
    int getDrawPriority() override { return 1; }

private:
    static constexpr float gravity = 0.075;
    static constexpr float maxYSpeed = 2;
    static constexpr float drag = 0.015;

    TA_Point velocity;
    TA_Sound explosionSound;
    float timer = 0;
    bool destroyed = false;
};
