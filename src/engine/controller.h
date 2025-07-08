#ifndef TA_CONTROLLER_H
#define TA_CONTROLLER_H

#include <array>
#include "gamepad.h"
#include "keyboard.h"
#include "sprite.h"
#include "tools.h"
#include "touchscreen.h"

class TA_GamepadController {
public:
    TA_Point getDirectionVector() { return TA::gamepad::getDirectionVector(); }
    bool isPressed(TA_FunctionButton button) { return TA::gamepad::isPressed(button); }
    bool isJustPressed(TA_FunctionButton button) { return TA::gamepad::isJustPressed(button); }
};

class TA_KeyboardController {
public:
    TA_Point getDirectionVector() { return TA::keyboard::getDirectionVector(); }
    bool isPressed(TA_FunctionButton button) { return TA::keyboard::isPressed(button); }
    bool isJustPressed(TA_FunctionButton button) { return TA::keyboard::isJustPressed(button); }
};

enum TA_OnscreenControllerMode {
    TA_ONSCREEN_CONTROLLER_DISABLED,
    TA_ONSCREEN_CONTROLLER_DPAD,
    TA_ONSCREEN_CONTROLLER_GAME,
    TA_ONSCREEN_CONTROLLER_PAUSE
};

class TA_OnscreenController {
private:
    void updatePositions();
    void updateStick();
    void setButtonPosition(TA_FunctionButton button, TA_Point center);
    void setArrowButtonPosition(TA_Direction button, TA_Point center);

    TA_OnscreenControllerMode mode = TA_ONSCREEN_CONTROLLER_DISABLED;

    std::array<TA_OnscreenButton, TA_BUTTON_MAX> buttons, arrowButtons;
    std::array<TA_Sprite, TA_BUTTON_MAX> sprites, arrowSprites;

    TA_Sprite stickSprite, pointerSprite;
    TA_OnscreenStick stick;
    TA_Point vector;

public:
    void load();
    void update();
    void draw();
    void setMode(TA_OnscreenControllerMode newMode) { mode = newMode; }
    void setAlpha(int alpha);

    TA_Point getDirectionVector();
    bool isPressed(TA_FunctionButton button);
    bool isJustPressed(TA_FunctionButton button);
};

class TA_Controller {
private:
    const float analogDeadZone = 0.25;
    const float verticalRange = 30;

    TA_GamepadController gamepad;
    TA_KeyboardController keyboard;
    TA_OnscreenController onscreen;

    TA_Direction currentDirection = TA_DIRECTION_MAX;
    bool justChanged = false;

public:
    void load();
    void update();
    void draw();
    void setMode(TA_OnscreenControllerMode mode) { onscreen.setMode(mode); }
    void setAlpha(int alpha) { onscreen.setAlpha(alpha); }

    TA_Point getDirectionVector();
    TA_Direction getDirection();

    bool isPressed(TA_FunctionButton button);
    bool isJustPressed(TA_FunctionButton button);
    bool isJustChangedDirection() { return justChanged; }
    bool isTouchscreen();
};

#endif // TA_CONTROLLER_H
