#include "SDL3/SDL.h"
#include "keyboard.h"
#include "save.h"

namespace TA { namespace keyboard {
    std::array<SDL_Scancode, TA_BUTTON_MAX> mapping;
    std::array<SDL_Scancode, TA_DIRECTION_MAX> directionMapping;
    std::array<bool, SDL_SCANCODE_COUNT> pressed, justPressed;
    
    std::array<bool, SDL_SCANCODE_COUNT> getKeyboardState();
    void updateMapping();
}}

void TA::keyboard::update()
{
    updateMapping();
    std::array<bool, SDL_SCANCODE_COUNT> keyboardState = getKeyboardState();

    for(int button = 0; button < SDL_SCANCODE_COUNT; button ++) {
        if(keyboardState[button]) {
            justPressed[button] = !pressed[button];
            pressed[button] = true;
        }
        else {
            pressed[button] = justPressed[button] = false;
        }
    }
}

void TA::keyboard::updateMapping()
{
    auto getMap = [] (std::string name) {
        return (SDL_Scancode)TA::save::getParameter("keyboard_map_" + name);
    };

    mapping[TA_BUTTON_A] = getMap("a");
    mapping[TA_BUTTON_B] = getMap("b");
    mapping[TA_BUTTON_PAUSE] = getMap("start");
    mapping[TA_BUTTON_LB] = getMap("lb");
    mapping[TA_BUTTON_RB] = getMap("rb");

    directionMapping[TA_DIRECTION_UP] = getMap("up");
    directionMapping[TA_DIRECTION_DOWN] = getMap("down");
    directionMapping[TA_DIRECTION_LEFT] = getMap("left");
    directionMapping[TA_DIRECTION_RIGHT] = getMap("right");
}

std::array<bool, SDL_SCANCODE_COUNT> TA::keyboard::getKeyboardState()
{
    if(TA::eventLog::isReading()) {
        std::array<bool, SDL_SCANCODE_COUNT> state;
        for(int button = 0; button < SDL_SCANCODE_COUNT; button ++) {
            state[button] = TA::eventLog::read();
        }

        return state;
    }

    else {
        std::array<bool, SDL_SCANCODE_COUNT> state;
        int* keyState = new int[SDL_SCANCODE_COUNT];
        SDL_GetKeyboardState(keyState);

        for(int button = 0; button < SDL_SCANCODE_COUNT; button ++) {
            state[button] = keyState[button];
        }

        if(TA::eventLog::isWriting()) {
            for(int button = 0; button < SDL_SCANCODE_COUNT; button ++) {
                TA::eventLog::write(state[button]);
            }
        }

        delete keyState;
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
