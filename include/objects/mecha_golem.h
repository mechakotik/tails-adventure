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
    const double waitTime = 20;
    const double goTime = 25;
    const double goBorder = 32;
    const double goDistance = 16;
    const double stepHeight = 8;
    const double armMoveMaxDistance = 64;
    const double armMoveTime = 16;
    const double phaseChangeTime = 120;
    const double phaseChangeExplosionInterval = 6;
    const double defeatedTime = 180;
    const double defeatedExplosionInterval = 6;
    const double gravity = 0.125;
    
    const double armBite1Time = 16;
    const double armBite2Time = 20;
    const double armBite3Time = 16;
    const double armBite4Time = 12;
    const double armCircleTime = 40;

    enum State {
        STATE_IDLE,
        STATE_WAIT,
        STATE_GO_LEFT,
        STATE_GO_RIGHT,
        STATE_ARM_MOVE,
        STATE_ARM_MOVE_BACK,
        STATE_ARM_CIRCLE,
        STATE_ARM_BITE1,
        STATE_ARM_BITE2,
        STATE_ARM_BITE3,
        STATE_ARM_BITE4,
        STATE_PHASE_CHANGE,
        STATE_BLOW,
        STATE_FALL,
        STATE_DEFEATED,
        STATE_WAIT_ITEM,
    };

    enum Hitbox {
        HITBOX_WALL_LEFT,
        HITBOX_WALL_RIGHT,
        HITBOX_BODY,
        HITBOX_WEAK,
        HITBOX_ARM,
        HITBOX_MAX
    };

    State state = STATE_IDLE, previousState = STATE_IDLE;

    void initGo();
    void initArmMove();
    void initPhaseChange();
    void initBlow();

    void updateIdle();
    void updateWait();
    void updateGo();
    void updateArmMove();
    void updateArmMoveBack();
    void updatePhaseChange();
    TA_Point getOptimalArmTarget();
    void updateBlow();
    void updateFall();
    void updateDefeated();

    void updateArmBite1();
    void updateArmBite2();
    void updateArmBite3();
    void updateArmBite4();
    void updateArmCircle();

    bool isComplete();
    bool canDoTransition();
    void doTransition();

    void updateDamage();
    void updateHitboxes();
    void drawArm();

    TA_Sprite headSprite, bodySprite, leftFootSprite, rightFootSprite, headFlashSprite;
    TA_Sprite armSprite, armPartSprite;
    TA_Sound hitSound, explosionSound, smallExplosionSound;

    TA_Point armPosition, armTarget;
    double timer = 0;
    double startX = 0;
    double speed = 0;

    double invincibleTimer = invincibleTime;
    int health = 24;
    bool secondPhase = false;
};

#endif
