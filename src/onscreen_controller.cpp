#include "controller.h"

void TA_OnscreenController::load()
{
    sprites[TA_BUTTON_A].load("controls/a_button.png");
    updatePositions();
}

void TA_OnscreenController::update()
{
    updatePositions();
    for(int pos = 0; pos < TA_BUTTON_MAX; pos ++) {
        buttons[pos].update();
    }
}

void TA_OnscreenController::draw()
{
    for(int pos = 0; pos < TA_BUTTON_MAX; pos ++) {
        sprites[pos].draw();
    }
}

void TA_OnscreenController::updatePositions()
{
    setFunctionButtonPosition(TA_BUTTON_A, TA_Point(TA::screenWidth - 20, TA::screenHeight - 20));
}

void TA_OnscreenController::setFunctionButtonPosition(TA_FunctionButton button, TA_Point center)
{
    buttons[button].setCircle(center, 12);
    sprites[button].setPosition(center - TA_Point(sprites[button].getWidth() / 2, sprites[button].getHeight() / 2));
}

TA_Point TA_OnscreenController::getDirectionVector()
{
    return TA_Point(0, 0);
}

bool TA_OnscreenController::isPressed(TA_FunctionButton button)
{
    return buttons[button].isPressed();
}

bool TA_OnscreenController::isJustPressed(TA_FunctionButton button)
{
    return buttons[button].isJustPressed();
}
