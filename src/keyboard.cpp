#include "keyboard.h"
#include "error.h"

namespace TA { namespace keyboard {
    std::array<SDL_Scancode, TA_BUTTON_MAX> mapping;
    std::array<SDL_Scancode, TA_DIRECTION_MAX> directionMapping;
    std::array<bool, SDL_NUM_SCANCODES> pressed, justPressed;
    std::array<bool, SDL_NUM_SCANCODES> getKeyboardState();
}}

void TA::keyboard::init()
{
    mapping[TA_BUTTON_A] = SDL_SCANCODE_Z;
    mapping[TA_BUTTON_B] = SDL_SCANCODE_C;
    mapping[TA_BUTTON_PAUSE] = SDL_SCANCODE_RETURN;
    mapping[TA_BUTTON_LB] = SDL_SCANCODE_A;
    mapping[TA_BUTTON_RB] = SDL_SCANCODE_D;

    directionMapping[TA_DIRECTION_UP] = SDL_SCANCODE_UP;
    directionMapping[TA_DIRECTION_DOWN] = SDL_SCANCODE_DOWN;
    directionMapping[TA_DIRECTION_LEFT] = SDL_SCANCODE_LEFT;
    directionMapping[TA_DIRECTION_RIGHT] = SDL_SCANCODE_RIGHT;
}

void TA::keyboard::update()
{
    std::array<bool, SDL_NUM_SCANCODES> keyboardState = getKeyboardState();

    for(int button = 0; button < SDL_NUM_SCANCODES; button ++) {
        if(keyboardState[button]) {
            justPressed[button] = !pressed[button];
            pressed[button] = true;
        }
        else {
            pressed[button] = justPressed[button] = false;
        }
    }
}

std::array<bool, SDL_NUM_SCANCODES> TA::keyboard::getKeyboardState()
{
    if(TA::eventLog::isReading()) {
        std::array<bool, SDL_NUM_SCANCODES> state;
        for(int button = 0; button < SDL_NUM_SCANCODES; button ++) {
            state[button] = TA::eventLog::read();
        }

        return state;
    }

    else {
        std::array<bool, SDL_NUM_SCANCODES> state;
        const uint8_t* keyState = SDL_GetKeyboardState(NULL);

        for(int button = 0; button < SDL_NUM_SCANCODES; button ++) {
            state[button] = keyState[button];
        }

        if(TA::eventLog::isWriting()) {
            for(int button = 0; button < SDL_NUM_SCANCODES; button ++) {
                TA::eventLog::write(state[button]);
            }
        }

        return state;
    }
}

bool TA::keyboard::isPressed(TA_FunctionButton button)
{
    return pressed[mapping[button]];
}

bool TA::keyboard::isJustPressed(TA_FunctionButton button)
{
    return justPressed[mapping[button]];
}

bool TA::keyboard::isScancodePressed(SDL_Scancode scancode)
{
    return pressed[scancode];
}

bool TA::keyboard::isScancodeJustPressed(SDL_Scancode scancode)
{
    return justPressed[scancode];
}

TA_Point TA::keyboard::getDirectionVector()
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
