#ifndef TA_CHARACTER_H
#define TA_CHARACTER_H

#include "controller.h"
#include "geometry.h"
#include "links.h"
#include "pawn.h"
#include "sound.h"
#include "tilemap.h"

class TA_Character : public TA_Pawn {
private:
    enum CharacterTool {
        TOOL_BOMB = 0,
        TOOL_REMOTE_BOMB = 2,
        TOOL_TELEPORT_DEVICE = 3,
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

    const float jmp = -3.75;
    const float grv = 0.125;
    const float acc = 0.25;
    const float airAcc = 0.09;
    const float topX = 1;
    const float minJumpSpeed = -2.5;
    const float maxJumpSpeed = 3;
    const float releaseJumpSpeed = -1.5;
    const float helitailAcc = 0.1;
    const float helitailTop = 1;
    const float hurtXsp = 1;
    const float hurtYsp = -2.5;
    const float invincibleTime = 120;
    const float springYsp = -8;
    const float remoteRobotInitTime = 30;
    const float remoteRobotJumpSpeed = -3.25;
    const float maxLookTime = 100;
    const float strongWindForce = 0.25;
    const float teleportInitTime = 60;
    const float hammerFallSpeed = 0.2;
    const float waterFriction = 0.75;
    const float waterFlowAcc = 0.15;
    const float maxCoyoteTime = 10;

    TA_Point followPosition, velocity, climbPosition;
    TA_Links links;
    TA_Polygon hitbox, hammerHitbox;
    TA_Point topLeft, bottomRight;
    TA_Point windVelocity;

    TA_Sound jumpSound, remoteRobotStepSound;
    TA_Sound flySound, remoteRobotFlySound;
    TA_Sound damageSound, hammerSound, waterSound, teleportSound;

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

    float jumpSpeed = 0, jumpTime = 0;
    float climbTime = 0, helitailTime = 0, invincibleTimeLeft = -1;
    float timer = 0, lookTime = 0, teleportTime = 0;
    float coyoteTime = 0;
    float deltaX = 0;
    int rings, currentTool = TOOL_BOMB;
    bool usingSpeedBoots = false;

    void physicsStep();
    void updateGround();
    void updateAir();
    void updateHelitail();
    void initHelitail();
    void updateWaterFlow();

    void updateFollowPosition();
    void horizontalMove();
    bool checkPawnCollision(TA_Polygon& hitbox) override;
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
    void spawnRemoteRobot();
    void initHammer();
    void updateHammer();
    void initTeleport();
    void updateTeleport();
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

    TA_Polygon* getHitbox() { return &hitbox; }
    TA_Polygon* getHammerHitbox() { return &hammerHitbox; }
    void setSpawnPoint(TA_Point newPosition, bool newFlip);
    TA_Point getPosition() { return position; }
    TA_Point getVelocity() { return velocity; }
    float getJumpTime() { return jumpTime; }

    void setCharacterPosition(TA_Point position);

    bool displayFlightTimeBar();
    float getFlightTime() { return helitailTime / getMaxHelitailTime(); }
    bool gameOver() { return state == STATE_DEAD && invincibleTimeLeft <= 0; }
    bool isRemoteRobot() { return remoteRobot; }
    bool isFlying() { return helitail; }
    bool isOnCeiling() { return ceiling; }
    bool isOnStrongWind() { return strongWind; }
    bool isTeleported();
    bool isUsingSpeedBoots() { return usingSpeedBoots; }
    bool isUsingHammer() { return state == STATE_HAMMER; }
    bool isInWater() { return water; }
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
