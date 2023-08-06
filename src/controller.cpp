#include <algorithm>
#include "controller.h"
#include "touchscreen.h"
#include "error.h"
#include "tools.h"

TA_Direction TA_Controller::getDirection()
{
    TA_Point vector = getDirectionVector();
    if(vector.length() < analogDeadZone) {
        return TA_DIRECTION_MAX;
    }
    double angle = atan2(vector.y, vector.x) * 180 / (atan(1) * 4);
    if(-90 - verticalRange <= angle && angle <= -90 + verticalRange) {
        return TA_DIRECTION_UP;
    }
    if(90 - verticalRange <= angle && angle <= 90 + verticalRange) {
        return TA_DIRECTION_DOWN;
    }
    if(vector.x > 0) {
        return TA_DIRECTION_RIGHT;
    }
    return TA_DIRECTION_LEFT;
}

void TA_CommonController::update()
{
    if(getDirection() != currentDirection) {
        currentDirection = getDirection();
        justChanged = true;
    }
    else {
        justChanged = false;
    }
}

TA_Point TA_CommonController::getDirectionVector()
{
    if(gamepad.getDirection() != TA_DIRECTION_MAX) {
        return gamepad.getDirectionVector();
    }
    return keyboard.getDirectionVector();
}

TA_Direction TA_CommonController::getDirection()
{
    if(gamepad.getDirection() != TA_DIRECTION_MAX) {
        return gamepad.getDirection();
    }
    return keyboard.getDirection();
}

bool TA_CommonController::isPressed(TA_FunctionButton button)
{
    return keyboard.isPressed(button) || gamepad.isPressed(button);
}

bool TA_CommonController::isJustPressed(TA_FunctionButton button)
{
    return keyboard.isJustPressed(button) || gamepad.isJustPressed(button);
}
