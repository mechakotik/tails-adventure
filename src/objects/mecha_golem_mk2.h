#ifndef TA_MECHA_GOLEM_MK2_H
#define TA_MECHA_GOLEM_MK2_H

#include "object_set.h"

class TA_MechaGolemMk2 : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position, TA_Point enterBlockerPosition, TA_Point exitBlockerPosition);
    bool update() override;
    void draw() override;

private:
    enum class State : uint8_t { WAIT, IDLE, PHASE_CHANGE, BLOW, DEFEATED };

    enum Hitbox : uint8_t {
        HITBOX_BODY,
        HITBOX_HEAD,
        HITBOX_ENTER_BLOCKER,
        HITBOX_EXIT_BLOCKER,
        HITBOX_MAX,
    };

    static constexpr float invincibleTime = 30;
    static constexpr float damageFlashTime = 5;
    static constexpr float phaseChangeTime = 120;
    static constexpr float phaseChangeExplosionInterval = 6;
    static constexpr float blowTime = 180;
    static constexpr float blowInterval = 6;

    void updateWait();
    void updateIdle();
    void initPhaseChange();
    void updatePhaseChange();
    void initBlow();
    void updateBlow();
    void initDefeated();
    void updateDamage();
    void updateEnterBlocker();
    void updateExitBlocker();

    State state = State::WAIT;
    bool secondPhase = false;

    TA_Sprite bodySprite;
    TA_Sprite headSprite;
    TA_Sprite headFlashSprite;
    TA_Sprite enterBlockerSprite;
    TA_Sprite exitBlockerSprite;

    TA_Point enterBlockerPosition;
    TA_Point exitBlockerPosition;
    float enterBlockerOffset = 32;
    float exitBlockerOffset = 0;

    TA_Sound hitSound;
    TA_Sound explosionSound;

    float timer = 0;

    int health = 32;
    float invincibleTimer = invincibleTime;
};

#endif
