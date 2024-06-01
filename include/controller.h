#ifndef TA_CONTROLLER_H
#define TA_CONTROLLER_H

#include <array>
#include "sprite.h"
#include "touchscreen.h"
#include "tools.h"
#include "gamepad.h"
#include "keyboard.h"

class TA_AbstractController {
private:
    double analogDeadZone = 0.25;
    double verticalRange = 30;

public:
    virtual void load() {}
    virtual void update() {}
    virtual void draw() {}

    virtual TA_Point getDirectionVector() {return TA_Point(0, 0);}
    virtual bool isPressed(TA_FunctionButton button) {return false;}
    virtual bool isJustPressed(TA_FunctionButton button) {return false;}

    TA_Direction getDirection();
};

class TA_GamepadController : public TA_AbstractController {
public:
    TA_Point getDirectionVector() override {return TA::gamepad::getDirectionVector();}
    bool isPressed(TA_FunctionButton button) override {return TA::gamepad::isPressed(button);}
    bool isJustPressed(TA_FunctionButton button) override {return TA::gamepad::isJustPressed(button);}
};

class TA_KeyboardController : public TA_AbstractController {
public:
    TA_Point getDirectionVector() override {return TA::keyboard::getDirectionVector();}
    bool isPressed(TA_FunctionButton button) override {return TA::keyboard::isPressed(button);}
    bool isJustPressed(TA_FunctionButton button) override {return TA::keyboard::isJustPressed(button);}
};

class TA_OnscreenController : public TA_AbstractController {
private:
    void updatePositions();
    void setFunctionButtonPosition(TA_FunctionButton button, TA_Point center);

    std::array<TA_Button, TA_BUTTON_MAX> buttons;
    std::array<TA_Sprite, TA_BUTTON_MAX> sprites;

public:
    void load() override;
    void update() override;
    void draw() override;

    TA_Point getDirectionVector() override;
    bool isPressed(TA_FunctionButton button) override;
    bool isJustPressed(TA_FunctionButton button) override;
};

class TA_Controller {
private:
    TA_GamepadController gamepad;
    TA_KeyboardController keyboard;
    TA_OnscreenController onscreen;

    TA_Direction currentDirection = TA_DIRECTION_MAX;
    bool justChanged = false;

public:
    void load();
    void update();
    void draw();

    TA_Point getDirectionVector();
    TA_Direction getDirection();

    bool isPressed(TA_FunctionButton button);
    bool isJustPressed(TA_FunctionButton button);
    bool isJustChangedDirection() {return justChanged;}
};

#endif // TA_CONTROLLER_H
