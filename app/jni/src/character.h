#ifndef TA_CHARACTER_H
#define TA_CHARACTER_H

#include "sprite.h"
#include "geometry.h"
#include "character_controller.h"
#include "links.h"
#include "tilemap.h"

class TA_Character {
private:
    const double jmp = -2.5;
    const double grv = 0.125;
    const double acc = 0.25;
    const double top = 1;

    TA_CharacterController controller;
    TA_Sprite sprite;
    TA_Point position, followPosition, speed;
    TA_GameScreenLinks links;

    bool ground = false;
    double minVectorPos;

    void updateFollowPosition();
    void verticalMove();
    void updateCollisions();

public:
    void load(TA_GameScreenLinks newLinks);
    void update();
    void draw() {sprite.draw();}
    void drawControls() {controller.draw();}
};

#endif // TA_CHARACTER_H
