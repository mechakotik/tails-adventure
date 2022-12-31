#ifndef TA_GAMEPAD_H
#define TA_GAMEPAD_H

#include "SDL.h"
#include "geometry.h"
#include "tools.h"

namespace TA { namespace gamepad {
    void init();
    void update();
    bool connected();
    void quit();

    TA_Point getDirectionVector();
    bool isPressed(TA_FunctionButton button);
    bool isJustPressed(TA_FunctionButton button);
}}

#endif // TA_GAMEPAD_H
