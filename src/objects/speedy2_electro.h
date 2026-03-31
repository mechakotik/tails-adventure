#ifndef TA_SPEEDY2_ELECTRO_H
#define TA_SPEEDY2_ELECTRO_H

#include "object_set.h"

class TA_Speedy2Electro : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    void draw() override;
    void updatePosition() override;

private:
    TA_Sprite foregroundSprite;
    TA_Sprite backgroundSprite;

    float timer = 0;
    float glowTimer = 0;
};

#endif
