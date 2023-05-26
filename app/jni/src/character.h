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
        TOOL_REMOTE_ROBOT = 6
    };

    enum TA_CharacterState {
        STATE_NORMAL, // TODO: create ground, air, helitail state
        STATE_CLIMB_LOW,
        STATE_CLIMB_HIGH,
        STATE_THROW_BOMB,
        STATE_REMOTE_ROBOT_INIT,
        STATE_REMOTE_ROBOT_RETURN,
        STATE_DEAD
    };

    const double jmp = -2.5;
    const double grv = 0.125;
    const double acc = 0.25;
    const double topX = 1;
    const double topY = 3;
    const double maxJumpTime = 10;
    const double helitailAcc = 0.05;
    const double helitailTop = 1;
    const double maxHelitailTime = 300;
    const double hurtXsp = 1;
    const double hurtYsp = -2.5;
    const double invincibleTime = 120;
    const double springYsp = -8;
    const double remoteRobotInitTime = 30;
    const double remoteRobotJumpSpeed = -2;

    TA_CommonController controller;
    TA_Point followPosition, velocity, climbPosition;
    TA_Links links;
    TA_Sound areaBeginSound, areaLoopSound, jumpSound;
    TA_Polygon hitbox;
    TA_Point topLeft, bottomRight;

    TA_CharacterState state = STATE_NORMAL;

    TA_Sprite remoteRobotControlSprite;
    TA_Point remoteRobotInitialPosition;

    bool ground = false, helitail = false, wall = false, flip = false;
    bool jump = false, jumpReleased = false, spring = false;
    bool hurt = false;
    bool lookUp = false, crouch = false;
    bool useHalfSolidTiles = false;
    bool remoteRobot = false;
    bool bombDestroySignal = false;

    double jumpTime = 0, climbTime = 0, helitailTime = 0, invincibleTimeLeft = -1, timer = 0;
    double deltaX = 0;
    int rings = 10;
    int currentTool = TOOL_BOMB;

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
    void updateTool();
    void updateObjectCollision();
    void updateRemoteRobotReturn();

public:
    void load(TA_Links newLinks);
    void handleInput();
    void update();
    void draw() override;
    void drawControls() {controller.draw();}

    bool isOnGround() {return ground;}
    bool isJumpingOnSpring() {return spring;}
    bool getBombDestroySignal() {return bombDestroySignal;}

    TA_Polygon *getHitbox() {return &hitbox;}
    TA_CommonController *getController() {return &controller;}
    void setSpawnPoint(TA_Point newPosition, bool newFlip);
    TA_Point getPosition() {return position + TA_Point(24, 24);}

    void addRings(int count);
    bool gameOver() {return state == STATE_DEAD && invincibleTimeLeft <= 0;}
};

#endif // TA_CHARACTER_H
