#include "keyboard.h"

namespace TA { namespace keyboard {
    std::array<SDL_Scancode, TA_BUTTON_MAX> mapping;
    std::array<SDL_Scancode, TA_DIRECTION_MAX> directionMapping;
    std::array<bool, TA_BUTTON_MAX> pressed, justPressed;
    std::array<bool, TA_DIRECTION_MAX> directionPressed;
}}

void TA::keyboard::init()
{
    mapping[TA_BUTTON_A] = SDL_SCANCODE_Z;
    mapping[TA_BUTTON_B] = SDL_SCANCODE_C;
    mapping[TA_BUTTON_PAUSE] = SDL_SCANCODE_RETURN;

    directionMapping[TA_DIRECTION_UP] = SDL_SCANCODE_UP;
    directionMapping[TA_DIRECTION_DOWN] = SDL_SCANCODE_DOWN;
    directionMapping[TA_DIRECTION_LEFT] = SDL_SCANCODE_LEFT;
    directionMapping[TA_DIRECTION_RIGHT] = SDL_SCANCODE_RIGHT;
}

void TA::keyboard::update()
{
    const uint8_t *keyboardState = SDL_GetKeyboardState(NULL);
    for(int button = 0; button < TA_BUTTON_MAX; button ++) {
        if(keyboardState[mapping[button]]) {
            justPressed[button] = !pressed[button];
            pressed[button] = true;
        }
        else {
            pressed[button] = justPressed[button] = false;
        }
    }

    for(int direction = 0; direction < TA_DIRECTION_MAX; direction ++) {
        directionPressed[direction] = bool(keyboardState[directionMapping[direction]]);
    }
}

bool TA::keyboard::isPressed(TA_FunctionButton button)
{
    return pressed[button];
}

bool TA::keyboard::isJustPressed(TA_FunctionButton button)
{
    return justPressed[button];
}

TA_Point TA::keyboard::getDirectionVector()
{
    int verticalDirection = -1, horizontalDirection = -1;
    for(int direction : {TA_DIRECTION_UP, TA_DIRECTION_DOWN}) {
        if(directionPressed[direction]) {
            verticalDirection = direction;
            break;
        }
    }
    for(int direction : {TA_DIRECTION_LEFT, TA_DIRECTION_RIGHT}) {
        if(directionPressed[direction]) {
            horizontalDirection = direction;
            break;
        }
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
