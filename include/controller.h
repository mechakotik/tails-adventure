#ifndef TA_CONTROLLER_H
#define TA_CONTROLLER_H

#include <array>
#include "sprite.h"
#include "touchscreen.h"
#include "tools.h"
#include "gamepad.h"
#include "keyboard.h"

class TA_Controller {
private:
    double analogDeadZone = 0.25;
    double verticalRange = 30;

public:
    virtual TA_Point getDirectionVector() {return TA_Point(0, 0);}
    TA_Direction getDirection();

    virtual bool isPressed(TA_FunctionButton button) {return false;}
    virtual bool isJustPressed(TA_FunctionButton button) {return false;}
};

class TA_GamepadController : public TA_Controller
{
public:
    TA_Point getDirectionVector() override {return TA::gamepad::getDirectionVector();}
    bool isPressed(TA_FunctionButton button) override {return TA::gamepad::isPressed(button);}
    bool isJustPressed(TA_FunctionButton button) override {return TA::gamepad::isJustPressed(button);}
};

class TA_KeyboardController : public TA_Controller
{
public:
    TA_Point getDirectionVector() override {return TA::keyboard::getDirectionVector();}
    bool isPressed(TA_FunctionButton button) override {return TA::keyboard::isPressed(button);}
    bool isJustPressed(TA_FunctionButton button) override {return TA::keyboard::isJustPressed(button);}
};

class TA_CommonController
{
private:
    TA_GamepadController gamepad;
    TA_KeyboardController keyboard;

    TA_Direction currentDirection = TA_DIRECTION_MAX;
    bool justChanged = false;

public:
    void load() {update();}
    void update();

    TA_Point getDirectionVector();
    TA_Direction getDirection();

    bool isPressed(TA_FunctionButton button);
    bool isJustPressed(TA_FunctionButton button);
    bool isJustChangedDirection() {return justChanged;}
};

#endif // TA_CONTROLLER_H
