#ifndef TA_CHARACTER_H
#define TA_CHARACTER_H

#include "geometry.h"
#include "links.h"
#include "sound.h"
#include "sprite.h"

class TA_Character : public TA_Sprite {
private:
    enum CharacterTool {
        TOOL_BOMB = 0,
        TOOL_LARGE_BOMB = 1,
        TOOL_REMOTE_BOMB = 2,
        TOOL_TELEPORT_DEVICE = 3,
        TOOL_NIGHT_VISION = 5,
        TOOL_REMOTE_ROBOT = 6,
        TOOL_SPEED_BOOTS = 7,
        TOOL_NAPALM_BOMB = 13,
        TOOL_TRIPLE_BOMB = 14,
        TOOL_RADIO = 18,
        TOOL_HAMMER = 19
    };

    enum CharacterState {
        STATE_NORMAL, // TODO: create ground, air, helitail state
        STATE_CLIMB_LOW,
        STATE_CLIMB_HIGH,
        STATE_THROW_BOMB,
        STATE_REMOTE_ROBOT_INIT,
        STATE_REMOTE_ROBOT_RETURN,
        STATE_UNPACK_ITEM,
        STATE_RAISE_ITEM,
        STATE_TELEPORT,
        STATE_HAMMER,
        STATE_DEAD
    };

    enum BombType { BOMB_REGULAR, BOMB_REMOTE, BOMB_NAPALM, BOMB_TRIPLE };

    static constexpr float jmp = -3.75;
    static constexpr float grv = 0.125;
    static constexpr float acc = 0.25;
    static constexpr float airAcc = 0.09;
    static constexpr float topX = 1;
    static constexpr float minJumpSpeed = -2.5;
    static constexpr float maxJumpSpeed = 3;
    static constexpr float releaseJumpSpeed = -1.5;
    static constexpr float helitailAcc = 0.1;
    static constexpr float helitailTop = 1;
    static constexpr float hurtXsp = 1;
    static constexpr float hurtYsp = -2.5;
    static constexpr float invincibleTime = 120;
    static constexpr float springYsp = -8;
    static constexpr float remoteRobotInitTime = 30;
    static constexpr float remoteRobotJumpSpeed = -3.25;
    static constexpr float maxLookTime = 100;
    static constexpr float strongWindForce = 0.25;
    static constexpr float teleportInitTime = 60;
    static constexpr float hammerFallSpeed = 0.2;
    static constexpr float waterFriction = 0.75;
    static constexpr float waterFlowAcc = 0.15;
    static constexpr float maxCoyoteTime = 10;
    static constexpr float nightVisionActivateTime = 10;

    TA_Point position, followPosition, velocity, climbPosition;
    TA_Links links;
    TA_Rect hitbox, hammerHitbox;
    TA_Point topLeft, bottomRight;
    TA_Point windVelocity;

    TA_Sound jumpSound, remoteRobotStepSound;
    TA_Sound flySound, remoteRobotFlySound;
    TA_Sound damageSound, hammerSound, waterSound, teleportSound;
    TA_Sound nightVisionSound;

    CharacterState state = STATE_NORMAL;

    TA_Sprite remoteRobotControlSprite;
    TA_Point remoteRobotInitialPosition;

    bool ground = false, helitail = false, wall = false, ceiling = false, flip = false;
    bool jump = false, jumpReleased = false, spring = false;
    bool hurt = false;
    bool lookUp = false, crouch = false;
    bool useSolidUpTiles = false, useSolidDownTiles = false, useMovingPlatforms = true;
    bool remoteRobot = false;
    bool bombDestroySignal = false;
    bool strongWind = false;
    bool hidden = false, nextFrameHidden = false;
    bool water = false;
    bool conveyorBelt = false;

    float jumpSpeed = 0, jumpTime = 0;
    float climbTime = 0, helitailTime = 0, invincibleTimeLeft = -1;
    float timer = 0, lookTime = 0, teleportTime = 0;
    float coyoteTime = 0;
    float deltaX = 0;
    int rings, currentTool = TOOL_BOMB;
    bool usingSpeedBoots = false;

    float nightVisionTimer = 0;
    bool usingNightVision = false;

    bool debugMode = false;
    bool noclip = false;

    void physicsStep();
    void updateGround();
    void updateAir();
    void updateHelitail();
    void initHelitail();
    void updateWaterFlow();
    int getSolidFlags();

    void updateFollowPosition();
    void horizontalMove();
    void updateCollisions();
    void updateAnimation();
    void updateClimb();
    void updateClimbAnimation();
    void updateThrowAnimation();
    void updateSpringCollision();
    void updateWaterCollision();
    void updateRemoteRobotReturn();

    void updateTool();
    void spawnBomb(BombType type);
    void spawnLargeBomb();
    void spawnRemoteRobot();
    void initHammer();
    void updateHammer();
    void initTeleport();
    void updateTeleport();
    void initNightVision();
    void updateNightVision();
    void changeMusic();
    float getMaxHelitailTime();
    void dropRings();

public:
    void load(TA_Links newLinks);
    void handleInput();
    void update();
    void draw() override;

    bool isOnGround() { return ground; }
    bool isJumpingOnSpring() { return spring; }
    bool getBombDestroySignal() { return bombDestroySignal; }
    bool isClimbing() { return state == STATE_CLIMB_LOW || state == STATE_CLIMB_HIGH; }

    TA_Rect* getHitbox() { return &hitbox; }
    TA_Rect* getHammerHitbox() { return &hammerHitbox; }
    void setSpawnPoint(TA_Point newPosition, bool newFlip);
    TA_Point getPosition() { return position; }
    TA_Point getVelocity() { return velocity; }
    float getJumpTime() { return jumpTime; }

    void setCharacterPosition(TA_Point position);
    void restoreFollowPosition();

    bool displayFlightTimeBar();
    float getFlightTime() { return helitailTime / getMaxHelitailTime(); }
    bool gameOver() { return state == STATE_DEAD && invincibleTimeLeft <= 0; }
    bool isRemoteRobot() { return remoteRobot; }
    bool isFlying() { return helitail; }
    bool isOnCeiling() { return ceiling; }
    bool isTeleported();
    bool isUsingHammer() { return state == STATE_HAMMER; }
    bool isFastCamera() { return spring || strongWind || usingSpeedBoots || conveyorBelt || noclip; }
    bool isInWater() { return water; }
    bool isNightVisionApplied() { return nightVisionTimer > 0; }
    bool isGettingItem() {
        return state == STATE_UNPACK_ITEM || state == STATE_RAISE_ITEM || getAnimationName() == "release";
    }

    void setUnpackState() { state = STATE_UNPACK_ITEM; }
    void setRaiseState();
    void setReleaseState();
    void setHide(bool hidden) { nextFrameHidden = hidden; }
    bool isHidden() { return hidden; }

    void addToPosition(TA_Point delta) { position = position + delta; }
    void resetInstaShield();
    void setWindVelocity(TA_Point windVelocity);
    void setPaused(bool paused);
};

#endif // TA_CHARACTER_H
