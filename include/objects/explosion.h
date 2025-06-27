#ifndef TA_EXPLOSION_H
#define TA_EXPLOSION_H

#include "geometry.h"
#include "object_set.h"

enum TA_ExplosionType { TA_EXPLOSION_CHARACTER, TA_EXPLOSION_ENEMY, TA_EXPLOSION_NEUTRAL, TA_EXPLOSION_LARGE };

class TA_Explosion : public TA_Object {
private:
    TA_Point velocity;
    float timer = 0;
    int delay = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, int newDelay = 0, TA_ExplosionType type = TA_EXPLOSION_CHARACTER,
        TA_Point velocity = {0, 0});
    bool update() override;
    void draw() override;
    int getDrawPriority() override { return 1; }
};

#endif // TA_EXPLOSION_H
