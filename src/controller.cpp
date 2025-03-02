#include "controller.h"
#include <algorithm>
#include "error.h"
#include "tools.h"
#include "touchscreen.h"

void TA_Controller::load() {
#ifdef __ANDROID__
    onscreen.load();
#endif

    update();
}

void TA_Controller::update() {
#ifdef __ANDROID__
    onscreen.update();
#endif

    if(getDirection() != currentDirection) {
        currentDirection = getDirection();
        justChanged = true;
    } else {
        justChanged = false;
    }
}

void TA_Controller::draw() {
#ifdef __ANDROID__
    if(isTouchscreen()) {
        onscreen.draw();
    }
#endif
}

TA_Point TA_Controller::getDirectionVector() {
    for(TA_Point vector : {onscreen.getDirectionVector(), gamepad.getDirectionVector()}) {
        if(vector.length() >= analogDeadZone) {
            return vector;
        }
    }
    return keyboard.getDirectionVector();
}

TA_Direction TA_Controller::getDirection() {
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

bool TA_Controller::isPressed(TA_FunctionButton button) {
    return keyboard.isPressed(button) || gamepad.isPressed(button) || onscreen.isPressed(button);
}

bool TA_Controller::isJustPressed(TA_FunctionButton button) {
    return keyboard.isJustPressed(button) || gamepad.isJustPressed(button) || onscreen.isJustPressed(button);
}

bool TA_Controller::isTouchscreen() {
#ifdef __ANDROID__
    return !(TA::gamepad::connected() && TA::gamepad::oncePressed());
#else
    return false;
#endif
}
