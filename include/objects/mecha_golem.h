#ifndef TA_MECHA_GOLEM_H
#define TA_MECHA_GOLEM_H

#include "object_set.h"

class TA_MechaGolem : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load();
    bool update() override;
    void draw() override;

private:
    const double invincibleTime = 30;
    const double damageFlashTime = 5;

    enum State {
        STATE_IDLE,
        STATE_P1_WAIT,
    };

    State state = STATE_IDLE;

    void updateIdle();
    void updateP1Wait();

    void updateDamage();
    void updateHitboxes();
    void insertBorderHitboxes();

    TA_Sprite headSprite, bodySprite, leftFootSprite, rightFootSprite, headFlashSprite;
    TA_Polygon weakHitbox;
    TA_Sound hitSound;

    double invincibleTimer = invincibleTime;
};

#endif
