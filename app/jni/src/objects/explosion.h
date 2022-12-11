#ifndef TA_EXPLOSION_H
#define TA_EXPLOSION_H

#include "engine/object_set.h"

class TA_Explosion : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
};

#endif // TA_EXPLOSION_H
