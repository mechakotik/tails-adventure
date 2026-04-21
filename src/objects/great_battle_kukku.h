#ifndef TA_GREAT_BATTLE_KUKKU_H
#define TA_GREAT_BATTLE_KUKKU_H

#include "object_set.h"

class TA_GreatBattleKukku : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point origin);
    bool update() override;
    void draw() override;

private:
    enum class State : uint8_t {
        WAIT,
        CHARACTER_FLY,
        APPROACH,
        LAUGH,
        MOVE,
        CROUCH_PHASE1,
        CROUCH_PHASE2,
        CAPTURE,
        THROW,
        DESTROYED,
        WAIT_GROUND,
        CHARACTER_EXIT,
        CHARACTER_JUMP,
        CHARACTER_FLY_AWAY,
    };

    static constexpr float bombPeriod = 180;

    void updateWait();
    void updateCharacterFly();
    void updateApproach();
    void updateLaugh();
    void updateMove();
    void updateCrouchPhase1();
    void updateCrouchPhase2();
    void updateCapture();
    void updateThrow();
    void updateDestroyed();
    void updateWaitGround();
    void updateCharacterExit();
    void updateCharacterJump();
    void updateCharacterFlyAway();

    void updateFollowPosition();

    State state = State::WAIT;
    TA_Point origin;

    TA_Sprite characterMock;
    TA_Sprite seaFoxMock;
    TA_Sprite flashSprite;
    TA_Sprite armSprite;
    TA_Sprite exitBlockerSprite;

    TA_Rect captureHitbox;

    TA_Point mockPosition;
    TA_Point followPosition;
    float mockYSpeed = 0;

    float timer = 0;
    float invincibleTimer = 0;
    float soundTimer = 0;
    float captureTimer = 0;
    float timeUntilCrouch = 0;
    float throwTargetX = 0;
    int health = 24;
    int crouchBombsLeft = 0;
    bool moveRight = false;
    bool mockMoved = false;

    std::array<float, 3> bombTimers{
        0,
        30,
        110,
    };

    std::array<TA_Point, 3> bombVelocities{
        TA_Point(-1.2F, -4.5F),
        TA_Point(-1.2F, -3.0F),
        TA_Point(-1.2F, -3.75F),
    };

    TA_Sound hitSound;
    TA_Sound explosionSound;
    TA_Sound laughSound;
    TA_Sound jumpSound;
};

#endif
