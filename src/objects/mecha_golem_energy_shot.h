#ifndef TA_MECHA_GOLEM_ENERGY_SHOT_H
#define TA_MECHA_GOLEM_ENERGY_SHOT_H

#include "object_set.h"

class TA_MechaGolemEnergyShot : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    void draw() override;
    void updatePosition() override;

private:
    TA_Sprite foregroundSprite;
    TA_Sprite backgroundSprite;
    float angle = 0;

    float glowTimer = 0;
};

#endif
