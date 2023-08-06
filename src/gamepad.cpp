#include <array>
#include "gamepad.h"
#include "save.h"
#include "tools.h"

namespace TA { namespace gamepad {
    void updateMapping();
    bool isDpadPressed();
    TA_Point getDpadDirectionVector();
    TA_Point getStickDirectionVector();

    std::array<SDL_GameControllerButton, TA_BUTTON_MAX> mapping;
    std::array<SDL_GameControllerButton, TA_DIRECTION_MAX> directionMapping;

    SDL_GameController *controller = nullptr;
    std::array<bool, SDL_CONTROLLER_BUTTON_MAX> pressed, justPressed;
    bool isConnected = false;
}}

bool TA::gamepad::connected()
{
    return isConnected;
}

void TA::gamepad::handleEvent(SDL_ControllerDeviceEvent event)
{
    if(event.type == SDL_CONTROLLERDEVICEADDED && !isConnected) {
        isConnected = true;
        init(event.which);
    }
    else if(event.type == SDL_CONTROLLERDEVICEREMOVED) {
        isConnected = false;
        quit();
    }
}

void TA::gamepad::init(int index)
{
    controller = SDL_GameControllerOpen(index);
    if(controller == nullptr) {
        isConnected = false;
        return;
    }
    else {
        isConnected = true;
    }
    SDL_GameControllerAddMappingsFromFile("controllerdb/gamecontrollerdb.txt");

    updateMapping();
}

void TA::gamepad::updateMapping()
{
    auto getMap = [] (std::string name) {
        return (SDL_GameControllerButton)TA::save::getParameter("gamepad_map_" + name);
    };

    mapping[TA_BUTTON_A] = getMap("a");
    mapping[TA_BUTTON_B] = getMap("b");
    mapping[TA_BUTTON_PAUSE] = getMap("start");
    mapping[TA_BUTTON_LB] = getMap("lb");
    mapping[TA_BUTTON_RB] = getMap("rb");

    directionMapping[TA_DIRECTION_UP] = SDL_CONTROLLER_BUTTON_DPAD_UP;
    directionMapping[TA_DIRECTION_DOWN] = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    directionMapping[TA_DIRECTION_LEFT] = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    directionMapping[TA_DIRECTION_RIGHT] = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
}

void TA::gamepad::update()
{
    if(!connected()) {
        return;
    }
    SDL_GameControllerUpdate();
    updateMapping();

    for(int button = 0; button < SDL_CONTROLLER_BUTTON_MAX; button ++) {
        if(SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)button)) {
            if(!pressed[button]) {
                pressed[button] = justPressed[button] = true;
            }
            else {
                justPressed[button] = false;
            }
        }
        else {
            pressed[button] = justPressed[button] = false;
        }
    }
}

TA_Point TA::gamepad::getDirectionVector()
{
    if(!connected()) {
        return {0, 0};
    }

    if(isDpadPressed()) {
        return getDpadDirectionVector();
    }
    return getStickDirectionVector();
}

bool TA::gamepad::isDpadPressed()
{
    for(int direction = 0; direction < TA_DIRECTION_MAX; direction ++) {
        if(SDL_GameControllerGetButton(controller, directionMapping[direction])) {
            return true;
        }
    }
    return false;
}

TA_Point TA::gamepad::getDpadDirectionVector()
{
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
    const double length = 0.8;

    if(verticalDirection == -1 || horizontalDirection == -1) {
        return (verticalDirection != -1 ? vectors[verticalDirection] : vectors[horizontalDirection]);
    }
    return {vectors[horizontalDirection].x * length, vectors[verticalDirection].y * length};
}

TA_Point TA::gamepad::getStickDirectionVector()
{
    auto getAxis = [&](double &x, SDL_GameControllerAxis index) {
        x = SDL_GameControllerGetAxis(controller, index);
        x /= 32768;
    };
    TA_Point vector;
    getAxis(vector.x, SDL_CONTROLLER_AXIS_LEFTX);
    getAxis(vector.y, SDL_CONTROLLER_AXIS_LEFTY);
    return vector;
}

bool TA::gamepad::isPressed(TA_FunctionButton button)
{
    return isControllerButtonPressed(mapping[button]);
}

bool TA::gamepad::isJustPressed(TA_FunctionButton button)
{
    return isControllerButtonJustPressed(mapping[button]);
}

bool TA::gamepad::isControllerButtonPressed(SDL_GameControllerButton button)
{
    return connected() && pressed[button];
}

bool TA::gamepad::isControllerButtonJustPressed(SDL_GameControllerButton button)
{
    return connected() && justPressed[button];
}

void TA::gamepad::rumble(double lowFreqStrength, double highFreqStrength, int time)
{
    if(!connected() || !TA::save::getParameter("rumble")) {
        return;
    }
    SDL_GameControllerRumble(controller, 0xFFFF * lowFreqStrength, 0xFFFF * highFreqStrength, time * 1000 / 60);
}

void TA::gamepad::quit()
{
    SDL_GameControllerClose(controller);
}
