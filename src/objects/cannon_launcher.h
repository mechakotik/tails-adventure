#ifndef TA_CANNON_LAUNCHER
#define TA_CANNON_LAUNCHER

#include "object_set.h"

class TA_CannonLauncher : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position, bool flip);
    bool update() override;

private:
    float timer = 0;
    bool flip = false;
    bool lookUp = false;
};

#endif
