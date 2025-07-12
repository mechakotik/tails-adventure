#include "object_set.h"

class TA_Sniper : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;

private:
    static constexpr float fireTime = 20;

    void updateAim();
    void updateCrouch();
    void updateFlip();

    bool aim = false;
    bool flip = false;
    float timer = 0;
};
