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

    const double jmp = -3.75;
    const double grv = 0.125;
    const double acc = 0.25;
    const double airAcc = 0.09;
    const double topX = 1;
    const double minJumpSpeed = -2.5;
    const double maxJumpSpeed = 3;
    const double releaseJumpSpeed = -1.5;
    const double helitailAcc = 0.1;
    const double helitailTop = 1;
    const double hurtXsp = 1;
    const double hurtYsp = -2.5;
    const double invincibleTime = 120;
    const double springYsp = -8;
    const double remoteRobotInitTime = 30;
    const double remoteRobotJumpSpeed = -3.25;
    const double maxLookTime = 100;
    const double strongWindForce = 0.25;
    const double teleportInitTime = 60;
    const double hammerFallSpeed = 0.2;
    const double waterFriction = 0.75;
    const double waterFlowAcc = 0.15;
    const double maxCoyoteTime = 10;

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

    double jumpSpeed = 0, jumpTime = 0;
    double climbTime = 0, helitailTime = 0, invincibleTimeLeft = -1;
    double timer = 0, lookTime = 0, teleportTime = 0;
    double coyoteTime = 0;
    double deltaX = 0;
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
    double getMaxHelitailTime();
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
    double getJumpTime() { return jumpTime; }

    void setCharacterPosition(TA_Point position);

    bool displayFlightTimeBar();
    double getFlightTime() { return helitailTime / getMaxHelitailTime(); }
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
