#ifndef TA_WOOD_H
#define TA_WOOD_H

#include "object_set.h"

class TA_Wood : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    void draw() override;

private:
    static constexpr float gravity = 0.125;
    static constexpr float maxYSpeed = 3;

    void updateIdle();
    void updateAttack();

    TA_Sprite birdSprite;
    TA_Point startPosition;
    TA_Point velocity;
    bool idle = true;
    bool ground = false;
    bool bird = false;
};

#endif // TA_WOOD_H
