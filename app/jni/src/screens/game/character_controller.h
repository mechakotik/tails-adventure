#ifndef TA_CHARACTER_CONTROLLER_H
#define TA_CHARACTER_CONTROLLER_H

#include <array>
#include "engine/sprite.h"
#include "engine/touchscreen.h"

enum TA_Direction {
    TA_DIRECTION_UP,
    TA_DIRECTION_DOWN,
    TA_DIRECTION_LEFT,
    TA_DIRECTION_RIGHT,
    TA_DIRECTION_MAX
};

enum TA_FunctionButton {
    TA_BUTTON_A,
    TA_BUTTON_B,
    TA_BUTTON_PAUSE,
    TA_BUTTON_MAX
};

class TA_CharacterController {
private:
    struct TA_ControllerButton {
        TA_Sprite sprite;
        TA_Button button;
        int colorMod = 128;
    };

    std::array<TA_ControllerButton, TA_DIRECTION_MAX> dpad;
    std::array<TA_ControllerButton, TA_BUTTON_MAX> functionButtons;

    TA_Point dpadPosition{39, 105}, dpadScale{0.27, 0.27};
    std::array<TA_Point, TA_BUTTON_MAX> positions;
    std::array<TA_Point, TA_BUTTON_MAX> scales;

    double dpadInterval = 5;
    int alpha = 225;

    TA_Direction currentDirection = TA_DIRECTION_MAX;

public:
    void load();
    void update();
    void draw();
    void setDpadPosition(TA_Point position, TA_Point scale);
    void setFunctionButtonPosition(TA_FunctionButton button, TA_Point position, TA_Point scale);
    TA_Direction getDirection() {return currentDirection;}
    bool isPressed(TA_FunctionButton button) {return functionButtons[button].button.isPressed();}
    bool isJustPressed(TA_FunctionButton button) {return functionButtons[button].button.isJustPressed();}
};

#endif // TA_CHARACTER_CONTROLLER_H
