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
        arrows[pos].setAlpha(225);
    }
}

int TA_CharacterController::update()
{
    for(int pos = 0; pos < TA_DIRECTION_MAX; pos ++) {
        buttons[pos].update();
    }

    bool anythingPressed = false;
    for(int pos = 0; pos < TA_DIRECTION_MAX; pos ++) {
        if(buttons[pos].isJustPressed()) {
            currentDirection = TA_Direction(pos);
        }
        if(buttons[pos].isPressed()) {
            anythingPressed = true;
        }
    }
    if(!anythingPressed) {
        currentDirection = TA_DIRECTION_MAX;
        return TA_DIRECTION_MAX;
    }

    if(currentDirection == TA_DIRECTION_LEFT || currentDirection == TA_DIRECTION_RIGHT) {
        if(buttons[TA_DIRECTION_RIGHT].isPressed()) {
            return TA_DIRECTION_RIGHT;
        }
        return TA_DIRECTION_LEFT;
    }
    if(buttons[TA_DIRECTION_UP].isPressed()) {
        return TA_DIRECTION_UP;
    }
    return TA_DIRECTION_DOWN;
}

void TA_CharacterController::setPosition(TA_Point position, TA_Point scale)
{
    arrows[TA_DIRECTION_UP].setPosition(position + TA_Point(-30, -76 - interval) * scale);
    arrows[TA_DIRECTION_DOWN].setPosition(position + TA_Point(-30, interval) * scale);
    arrows[TA_DIRECTION_LEFT].setPosition(position + TA_Point(-76 - interval, -30) * scale);
    arrows[TA_DIRECTION_RIGHT].setPosition(position + TA_Point(interval, -30) * scale);

    auto addVertical = [&](double x, double y) {
        buttons[TA_DIRECTION_UP].addVertex(position + TA_Point(x, y) * scale);
        buttons[TA_DIRECTION_DOWN].addVertex(position + TA_Point(x, -y) * scale);
    };
    addVertical(-40, -200);
    addVertical(-40, -40);
    addVertical(0, 0);
    addVertical(40, -40);
    addVertical(40, -200);

    auto addHorizontal = [&](double x, double y) {
        buttons[TA_DIRECTION_RIGHT].addVertex(position + TA_Point(x, y) * scale);
        buttons[TA_DIRECTION_LEFT].addVertex(position + TA_Point(-x, y) * scale);
    };
    addHorizontal(0, 0);
    addHorizontal(40, -40);
    addHorizontal(40, -80);
    addHorizontal(200, -80);
    addHorizontal(200, 80);
    addHorizontal(40, 80);
    addHorizontal(40, 40);

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
