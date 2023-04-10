#include <array>
#include "gamepad.h"
#include "error.h"
#include "tools.h"

namespace TA { namespace gamepad {
    SDL_GameController *controller = nullptr;
    std::array<SDL_GameControllerButton, TA_BUTTON_MAX> mapping;
    std::array<bool, TA_BUTTON_MAX> pressed, justPressed;
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

    mapping[TA_BUTTON_A] = SDL_CONTROLLER_BUTTON_A;
    mapping[TA_BUTTON_B] = SDL_CONTROLLER_BUTTON_B;
    mapping[TA_BUTTON_PAUSE] = SDL_CONTROLLER_BUTTON_START;
}

void TA::gamepad::update()
{
    if(!connected()) {
        return;
    }
    SDL_GameControllerUpdate();
    for(int button = 0; button < TA_BUTTON_MAX; button ++) {
        if(SDL_GameControllerGetButton(controller, mapping[button])) {
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
    return pressed[button];
}

bool TA::gamepad::isJustPressed(TA_FunctionButton button)
{
    return justPressed[button];
}

void TA::gamepad::quit()
{
    SDL_GameControllerClose(controller);
}
