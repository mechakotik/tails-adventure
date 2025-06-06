#ifndef TA_KEYBOARD_H
#define TA_KEYBOARD_H

#include <array>
#include "SDL3/SDL.h"
#include "SDL3/SDL_scancode.h"
#include "geometry.h"
#include "tools.h"

namespace TA {
    namespace keyboard {
        void init();
        void update();
        bool isPressed(TA_FunctionButton button);
        bool isJustPressed(TA_FunctionButton button);
        bool isScancodePressed(SDL_Scancode scancode);
        bool isScancodeJustPressed(SDL_Scancode scancode);
        TA_Point getDirectionVector();
    }
}

#endif // TA_KEYBOARD_H
