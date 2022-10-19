#ifndef TA_CHARACTER_CONTROLLER_H
#define TA_CHARACTER_CONTROLLER_H

#include <array>
#include "sprite.h"
#include "touchscreen.h"

enum TA_Direction {
    TA_DIRECTION_UP,
    TA_DIRECTION_DOWN,
    TA_DIRECTION_LEFT,
    TA_DIRECTION_RIGHT,
    TA_DIRECTION_MAX
};

class TA_CharacterController {
private:
    std::array<TA_Sprite, TA_DIRECTION_MAX> arrows;
    std::array<TA_Button, TA_DIRECTION_MAX> buttons;

    TA_Point defaultPosition{39, 105}, defaultScale{0.27, 0.27};
    double interval = 5;
    TA_Direction currentDirection = TA_DIRECTION_MAX;

public:
    void load();
    int update();
    void draw();
    void setPosition(TA_Point position, TA_Point scale);
};


#endif // TA_CHARACTER_CONTROLLER_H
