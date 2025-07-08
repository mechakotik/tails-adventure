#include "object_set.h"

class TA_LargeBomb : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    int getDrawPriority() override { return 1; }

private:
    static constexpr float gravity = 0.125;
    static constexpr float explosionSpeed = 1.5;

    void explode();

    TA_Point velocity;
    TA_Sound explosionSound;
};
