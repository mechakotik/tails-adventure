#include "object_set.h"

class TA_MineLauncher : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    int getDrawPriority() override { return 1; }

private:
    static constexpr float cooldown = 180;

    float timer = cooldown + 1;
};
