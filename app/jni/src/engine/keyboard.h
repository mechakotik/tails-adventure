#ifndef TA_KEYBOARD_H
#define TA_KEYBOARD_H

#include <array>
#include "SDL.h"
#include "tools.h"
#include "geometry.h"

namespace TA { namespace keyboard {
    void init();
    void update();
    bool isPressed(TA_FunctionButton button);
    bool isJustPressed(TA_FunctionButton button);
    bool isScancodePressed(SDL_Scancode scancode);
    bool isScancodeJustPressed(SDL_Scancode scancode);
    TA_Point getDirectionVector();
}}

#endif // TA_KEYBOARD_H
