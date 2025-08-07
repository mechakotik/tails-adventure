#ifndef TA_MECHA_GOLEM_MK2_H
#define TA_MECHA_GOLEM_MK2_H

#include "object_set.h"

class TA_MechaGolemMk2 : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    void draw() override;

private:
    enum class State : uint8_t {
        WAIT,
        IDLE,
    };

    enum Hitbox : uint8_t {
        HITBOX_BODY,
        HITBOX_HEAD,
        HITBOX_MAX,
    };

    void updateWait();
    void updateIdle();

    State state = State::WAIT;

    TA_Sprite bodySprite;
    TA_Sprite headSprite;
};

#endif
