#ifndef TA_CHARACTER_H
#define TA_CHARACTER_H

#include "engine/pawn.h"
#include "engine/geometry.h"
#include "controller.h"
#include "engine/links.h"
#include "engine/tilemap.h"
#include "engine/sound.h"

enum TA_CharacterTool {
    TA_TOOL_BOMB
};

class TA_Character : public TA_Pawn {
private:
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

    TA_CommonController controller;
    TA_Point followPosition, velocity, climbPosition;
    TA_GameScreenLinks links;
    TA_Sound areaBeginSound, areaLoopSound, jumpSound, ringSound;
    TA_Polygon hitbox;
    TA_Point topLeft, bottomRight;

    bool ground = false, helitail = false, wall = false, flip = false;
    bool jump = false, jumpReleased = false;
    bool climb = false, climbHigh = false, throwing = false, hurt = false, dead = false;
    bool lookUp = false, crouch = false;
    bool useHalfSolidTiles = false;
    double jumpTime = 0, climbTime = 0, helitailTime = 0, invincibleTimeLeft = -1;
    double deltaX = 0;
    int rings = 1;
    int currentTool = TA_TOOL_BOMB;

    void updateGround();
    void updateAir();
    void updateHelitail();

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

public:
    void load(TA_GameScreenLinks newLinks);
    void handleInput();
    void update();
    void drawControls() {controller.draw();}
    bool isOnGround() {return ground;}
    TA_Polygon *getHitbox() {return &hitbox;}

    int getRingsCount() {return std::max(0, rings);}
    int getCurrentItem() {return 0;}
    bool gameOver() {return dead && invincibleTimeLeft <= 0;}
};

#endif // TA_CHARACTER_H
