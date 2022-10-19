#include "character_controller.h"
#include "touchscreen.h"
#include "error.h"

void TA_CharacterController::load()
{
    arrows[TA_DIRECTION_UP].load("controls/up.png");
    arrows[TA_DIRECTION_DOWN].load("controls/down.png");
    arrows[TA_DIRECTION_LEFT].load("controls/left.png");
    arrows[TA_DIRECTION_RIGHT].load("controls/right.png");
    setPosition(defaultPosition, defaultScale);

    for(int pos = 0; pos < TA_DIRECTION_MAX; pos ++) {
        arrows[pos].setAlpha(255);
    }
}

int TA_CharacterController::update()
{
    int state = 0;
    buttons[TA_DIRECTION_UP].update();
    if(buttons[TA_DIRECTION_UP].isPressed()) {
        state = 1;
    }
    return state;
}

void TA_CharacterController::setPosition(TA_Point position, TA_Point scale)
{
    arrows[TA_DIRECTION_UP].setPosition(position + TA_Point(-30, -76 - interval) * scale);
    arrows[TA_DIRECTION_DOWN].setPosition(position + TA_Point(-30, interval) * scale);
    arrows[TA_DIRECTION_LEFT].setPosition(position + TA_Point(-76 - interval, -30) * scale);
    arrows[TA_DIRECTION_RIGHT].setPosition(position + TA_Point(interval, -30) * scale);

    buttons[TA_DIRECTION_UP].addVertex(position + TA_Point(-30, -100 - interval) * scale);
    buttons[TA_DIRECTION_UP].addVertex(position + TA_Point(-30, -30 - interval) * scale);
    buttons[TA_DIRECTION_UP].addVertex(position + TA_Point(0, -interval) * scale);
    buttons[TA_DIRECTION_UP].addVertex(position + TA_Point(30, -30 - interval) * scale);
    buttons[TA_DIRECTION_UP].addVertex(position + TA_Point(30, -100 - interval) * scale);

    for(int pos = 0; pos < TA_DIRECTION_MAX; pos ++) {
        arrows[pos].setScale(scale);
    }
}

void TA_CharacterController::draw()
{
    for(int pos = 0; pos < TA_DIRECTION_MAX; pos ++) {
        arrows[pos].draw();
    }
}
