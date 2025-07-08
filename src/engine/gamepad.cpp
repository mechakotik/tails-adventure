#include "gamepad.h"
#include <array>
#include "save.h"
#include "tools.h"

namespace TA::gamepad {
    void updateMapping();
    bool isDpadPressed();
    TA_Point getDpadDirectionVector();
    TA_Point getStickDirectionVector();

    std::array<SDL_GamepadButton, TA_BUTTON_MAX> mapping;
    std::array<SDL_GamepadButton, TA_DIRECTION_MAX> directionMapping;

    SDL_Gamepad* controller = nullptr;
    std::array<bool, SDL_GAMEPAD_BUTTON_COUNT> pressed, justPressed;
    bool isConnected = false;
    bool isOncePressed = false;
}

bool TA::gamepad::connected() {
    return isConnected;
}

bool TA::gamepad::oncePressed() {
    return isOncePressed;
}

void TA::gamepad::handleEvent(SDL_GamepadDeviceEvent event) {
    if(event.type == SDL_EVENT_GAMEPAD_ADDED && !isConnected) {
        isConnected = true;
        init(event.which);
    } else if(event.type == SDL_EVENT_GAMEPAD_REMOVED) {
        isConnected = false;
        quit();
    }
}

void TA::gamepad::init(int index) {
    controller = SDL_OpenGamepad(index);
    if(controller == nullptr) {
        isConnected = false;
        return;
    } else {
        isConnected = true;
    }

    SDL_AddGamepadMappingsFromFile("gamecontrollerdb.txt");
    updateMapping();
}

void TA::gamepad::updateMapping() {
    auto getMap = [](std::string name) { return (SDL_GamepadButton)TA::save::getParameter("gamepad_map_" + name); };

    mapping[TA_BUTTON_A] = getMap("a");
    mapping[TA_BUTTON_B] = getMap("b");
    mapping[TA_BUTTON_PAUSE] = getMap("start");
    mapping[TA_BUTTON_LB] = getMap("lb");
    mapping[TA_BUTTON_RB] = getMap("rb");

    directionMapping[TA_DIRECTION_UP] = SDL_GAMEPAD_BUTTON_DPAD_UP;
    directionMapping[TA_DIRECTION_DOWN] = SDL_GAMEPAD_BUTTON_DPAD_DOWN;
    directionMapping[TA_DIRECTION_LEFT] = SDL_GAMEPAD_BUTTON_DPAD_LEFT;
    directionMapping[TA_DIRECTION_RIGHT] = SDL_GAMEPAD_BUTTON_DPAD_RIGHT;
}

void TA::gamepad::update() {
    if(!connected()) {
        return;
    }
    updateMapping();

    for(int button = 0; button < SDL_GAMEPAD_BUTTON_COUNT; button++) {
        if(SDL_GetGamepadButton(controller, (SDL_GamepadButton)button)) {
            if(!pressed[(int)button]) {
                pressed[(int)button] = justPressed[(int)button] = true;
                isOncePressed = true;
            } else {
                justPressed[(int)button] = false;
            }
        } else {
            pressed[(int)button] = justPressed[(int)button] = false;
        }
    }
}

TA_Point TA::gamepad::getDirectionVector() {
    if(!connected()) {
        return {0, 0};
    }

    if(isDpadPressed()) {
        return getDpadDirectionVector();
    }
    return getStickDirectionVector();
}

bool TA::gamepad::isDpadPressed() {
    for(int direction = 0; direction < TA_DIRECTION_MAX; direction++) {
        if(SDL_GetGamepadButton(controller, directionMapping[direction])) {
            return true;
        }
    }
    return false;
}

TA_Point TA::gamepad::getDpadDirectionVector() {
    int verticalDirection = -1, horizontalDirection = -1;
    for(int direction : {TA_DIRECTION_UP, TA_DIRECTION_DOWN}) {
        if(pressed[directionMapping[direction]]) {
            verticalDirection = direction;
            break;
        }
    }
    for(int direction : {TA_DIRECTION_LEFT, TA_DIRECTION_RIGHT}) {
        if(pressed[directionMapping[direction]]) {
            horizontalDirection = direction;
            break;
        }
    }

    if(verticalDirection == -1 && horizontalDirection == -1) {
        return {0, 0};
    }

    std::array<TA_Point, TA_DIRECTION_MAX> vectors;
    vectors[TA_DIRECTION_UP] = {0, -1};
    vectors[TA_DIRECTION_DOWN] = {0, 1};
    vectors[TA_DIRECTION_LEFT] = {-1, 0};
    vectors[TA_DIRECTION_RIGHT] = {1, 0};
    const float length = 0.8;

    if(verticalDirection == -1 || horizontalDirection == -1) {
        return (verticalDirection != -1 ? vectors[verticalDirection] : vectors[horizontalDirection]);
    }
    return (vectors[verticalDirection] + vectors[horizontalDirection]) * length;
}

TA_Point TA::gamepad::getStickDirectionVector() {
    auto getAxis = [&](float& x, SDL_GamepadAxis index) {
        x = SDL_GetGamepadAxis(controller, index);
        x /= 32768;
    };
    TA_Point vector;
    getAxis(vector.x, SDL_GAMEPAD_AXIS_LEFTX);
    getAxis(vector.y, SDL_GAMEPAD_AXIS_LEFTY);
    return vector;
}

bool TA::gamepad::isPressed(TA_FunctionButton button) {
    return isControllerButtonPressed(mapping[button]);
}

bool TA::gamepad::isJustPressed(TA_FunctionButton button) {
    return isControllerButtonJustPressed(mapping[button]);
}

bool TA::gamepad::isControllerButtonPressed(SDL_GamepadButton button) {
    return connected() && pressed[(int)button];
}

bool TA::gamepad::isControllerButtonJustPressed(SDL_GamepadButton button) {
    return connected() && justPressed[(int)button];
}

void TA::gamepad::rumble(float lowFreqStrength, float highFreqStrength, int time) {
    if(!connected() || !TA::save::getParameter("rumble")) {
        return;
    }
    SDL_RumbleGamepad(controller, 0xFFFF * lowFreqStrength, 0xFFFF * highFreqStrength, time * 1000 / 60);
}

void TA::gamepad::quit() {
    SDL_CloseGamepad(controller);
}
