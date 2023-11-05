#ifndef TA_CHARACTER_H
#define TA_CHARACTER_H

#include "pawn.h"
#include "geometry.h"
#include "controller.h"
#include "links.h"
#include "tilemap.h"
#include "sound.h"

class TA_Character : public TA_Pawn {
private:
    enum TA_CharacterTool {
        TOOL_BOMB = 0,
        TOOL_REMOTE_BOMB = 2,
        TOOL_TELEPORT_DEVICE = 3,
        TOOL_REMOTE_ROBOT = 6,
        TOOL_SPEED_BOOTS = 7,
        TOOL_RADIO = 18,
        TOOL_HAMMER = 19
    };

    enum TA_CharacterState {
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

    const double jmp = -3.75;
    const double grv = 0.125;
    const double acc = 0.25;
    const double topX = 1;
    const double minJumpSpeed = -2.5;
    const double maxJumpSpeed = 3;
    const double releaseJumpSpeed = -1.8;
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

    TA_Point followPosition, velocity, climbPosition;
    TA_Links links;
    TA_Polygon hitbox;
    TA_Point topLeft, bottomRight;
    TA_Point windVelocity;

    TA_Sound jumpSound, remoteRobotStepSound;
    TA_Sound flySound, remoteRobotFlySound;
    TA_Sound damageSound, hammerSound, deathSound, teleportSound;

    TA_CharacterState state = STATE_NORMAL;

    TA_Sprite remoteRobotControlSprite;
    TA_Point remoteRobotInitialPosition;

    bool ground = false, helitail = false, wall = false, ceiling = false, flip = false;
    bool jump = false, jumpReleased = false, spring = false;
    bool hurt = false;
    bool lookUp = false, crouch = false;
    bool useHalfSolidTiles = false, useMovingPlatforms = true;
    bool remoteRobot = false;
    bool bombDestroySignal = false;
    bool strongWind = false;
    bool hidden = false, nextFrameHidden = false;

    double jumpSpeed = 0, jumpTime = 0;
    double climbTime = 0, helitailTime = 0, invincibleTimeLeft = -1;
    double timer = 0, lookTime = 0, teleportTime = 0;
    double deltaX = 0;
    int rings = 10;
    int currentTool = TOOL_BOMB;
    bool usingSpeedBoots = false;

    void physicsStep();
    void updateGround();
    void updateAir();
    void updateHelitail();
    void initHelitail();

    void updateFollowPosition();
    void verticalMove();
    bool checkPawnCollision(TA_Polygon &hitbox) override;
    void updateCollisions();
    void updateAnimation();
    void updateClimb();
    void updateClimbAnimation();
    void updateThrowAnimation();
    void updateObjectCollision();
    void updateRemoteRobotReturn();

    void updateTool();
    void spawnBomb(bool remote);
    void spawnRemoteRobot();
    void initHammer();
    void updateHammer();
    void initTeleport();
    void updateTeleport();
    void changeMusic();

    int getEmeraldsCount();
    int getMaxRings() {return 10 * (getEmeraldsCount() + 1);}
    double getMaxHelitailTime();

public:
    void load(TA_Links newLinks);
    void handleInput();
    void update();
    void draw() override;

    bool isOnGround() {return ground;}
    bool isJumpingOnSpring() {return spring;}
    bool getBombDestroySignal() {return bombDestroySignal;}
    bool isClimbing() {return state == STATE_CLIMB_LOW || state == STATE_CLIMB_HIGH;}

    TA_Polygon *getHitbox() {return &hitbox;}
    void setSpawnPoint(TA_Point newPosition, bool newFlip);
    TA_Point getPosition() {return position;}
    TA_Point getVelocity() {return velocity;}
    double getJumpTime() {return jumpTime;}
    TA_CollisionType getCollisionType();

    void addRings(int count);
    void addRingsToMaximum();
    void setCharacterPosition(TA_Point position);

    bool displayFlighTimeBar();
    double getFlightTime() {return helitailTime / getMaxHelitailTime();}
    bool gameOver() {return state == STATE_DEAD && invincibleTimeLeft <= 0;}
    bool isRemoteRobot() {return remoteRobot;}
    bool isFlying() {return helitail;}
    bool isOnCeiling() {return ceiling;}
    bool isOnStrongWind() {return strongWind;}
    bool isTeleported();
    bool isUsingSpeedBoots() {return usingSpeedBoots;}

    void setUnpackState() {state = STATE_UNPACK_ITEM;}
    void setRaiseState();
    void setReleaseState();
    void setHide(bool hidden) {nextFrameHidden = hidden;}
    bool isHidden() {return hidden;}

    void addToPosition(TA_Point delta) {position = position + delta;}
    void resetInstaShield();
    void setWindVelociy(TA_Point windVelocity);
};

#endif // TA_CHARACTER_H
