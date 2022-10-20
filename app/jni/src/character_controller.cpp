#include <algorithm>
#include "character_controller.h"
#include "touchscreen.h"
#include "error.h"
#include "globals.h"

void TA_CharacterController::load()
{
    dpad[TA_DIRECTION_UP].sprite.load("controls/up.png");
    dpad[TA_DIRECTION_DOWN].sprite.load("controls/down.png");
    dpad[TA_DIRECTION_LEFT].sprite.load("controls/left.png");
    dpad[TA_DIRECTION_RIGHT].sprite.load("controls/right.png");
    setDpadPosition(dpadPosition, dpadScale);

    for(int pos = 0; pos < TA_DIRECTION_MAX; pos ++) {
        dpad[pos].sprite.setAlpha(alpha);
    }

    functionButtons[TA_BUTTON_A].sprite.load("controls/a_button.png");
    functionButtons[TA_BUTTON_B].sprite.load("controls/b_button.png");
    functionButtons[TA_BUTTON_PAUSE].sprite.load("controls/pause.png");

    for(int pos = 0; pos < TA_BUTTON_MAX; pos ++) {
        functionButtons[pos].sprite.setAlpha(alpha);
    }

    setFunctionButtonPosition(TA_BUTTON_A, {238, 118}, dpadScale);
    setFunctionButtonPosition(TA_BUTTON_B, {246, 92}, dpadScale);
    setFunctionButtonPosition(TA_BUTTON_PAUSE, {264, 14}, dpadScale);
}

void TA_CharacterController::update()
{
    for(int pos = 0; pos < TA_DIRECTION_MAX; pos ++) {
        dpad[pos].button.update();
    }
    for(int pos = 0; pos < TA_BUTTON_MAX; pos ++) {
        functionButtons[pos].button.update();
    }

    bool anythingPressed = false;
    for(int pos = 0; pos < TA_DIRECTION_MAX; pos ++) {
        if(dpad[pos].button.isJustPressed()) {
            currentDirection = TA_Direction(pos);
        }
        if(dpad[pos].button.isPressed()) {
            anythingPressed = true;
        }
    }

    if(anythingPressed) {
        if(currentDirection == TA_DIRECTION_LEFT || currentDirection == TA_DIRECTION_RIGHT) {
            if(dpad[TA_DIRECTION_RIGHT].button.isPressed()) {
                currentDirection = TA_DIRECTION_RIGHT;
            }
            else {
                currentDirection = TA_DIRECTION_LEFT;
            }
        }
        else {
            if(dpad[TA_DIRECTION_UP].button.isPressed()) {
                currentDirection = TA_DIRECTION_UP;
            }
            else {
                currentDirection = TA_DIRECTION_DOWN;
            }
        }
    }
    else {
        currentDirection = TA_DIRECTION_MAX;
    }

    auto updateColorMod = [&](int colorMod, bool pressed) {
        if(pressed) {
            colorMod = std::min(255, int(colorMod + 32 * gElapsedTime));
        }
        else {
            colorMod = std::max(128, int(colorMod - 32 * gElapsedTime));
        }
        return colorMod;
    };

    for(int pos = 0; pos < TA_DIRECTION_MAX; pos ++) {
        dpad[pos].colorMod = updateColorMod(dpad[pos].colorMod, (pos == currentDirection));
        dpad[pos].sprite.setColorMod(dpad[pos].colorMod);
    }
    for(int pos = 0; pos < TA_BUTTON_MAX; pos ++) {
        functionButtons[pos].colorMod = updateColorMod(functionButtons[pos].colorMod, isPressed(TA_FunctionButton(pos)));
        functionButtons[pos].sprite.setColorMod(functionButtons[pos].colorMod);
    }
}

void TA_CharacterController::setDpadPosition(TA_Point position, TA_Point scale)
{
    dpad[TA_DIRECTION_UP].sprite.setPosition(position + TA_Point(-30, -76 - dpadInterval) * scale);
    dpad[TA_DIRECTION_DOWN].sprite.setPosition(position + TA_Point(-30, dpadInterval) * scale);
    dpad[TA_DIRECTION_LEFT].sprite.setPosition(position + TA_Point(-76 - dpadInterval, -30) * scale);
    dpad[TA_DIRECTION_RIGHT].sprite.setPosition(position + TA_Point(dpadInterval, -30) * scale);

    auto addVertical = [&](double x, double y) {
        dpad[TA_DIRECTION_UP].button.addVertex(position + TA_Point(x, y) * scale);
        dpad[TA_DIRECTION_DOWN].button.addVertex(position + TA_Point(x, -y) * scale);
    };
    addVertical(-40, -200);
    addVertical(-40, -40);
    addVertical(0, 0);
    addVertical(40, -40);
    addVertical(40, -200);

    auto addHorizontal = [&](double x, double y) {
        dpad[TA_DIRECTION_RIGHT].button.addVertex(position + TA_Point(x, y) * scale);
        dpad[TA_DIRECTION_LEFT].button.addVertex(position + TA_Point(-x, y) * scale);
    };
    addHorizontal(0, 0);
    addHorizontal(40, -40);
    addHorizontal(40, -80);
    addHorizontal(200, -80);
    addHorizontal(200, 80);
    addHorizontal(40, 80);
    addHorizontal(40, 40);

    for(int pos = 0; pos < TA_DIRECTION_MAX; pos ++) {
        dpad[pos].sprite.setScale(scale);
    }
}

void TA_CharacterController::setFunctionButtonPosition(TA_FunctionButton button, TA_Point position, TA_Point scale)
{
    functionButtons[button].sprite.setPosition(position + TA_Point(-40, -40) * scale);
    functionButtons[button].sprite.setScale(scale);
    functionButtons[button].button.setCircle(position, 50 * scale.x);
}

void TA_CharacterController::draw()
{
    for(int pos = 0; pos < TA_DIRECTION_MAX; pos ++) {
        dpad[pos].sprite.draw();
    }
    for(int pos = 0; pos < TA_BUTTON_MAX; pos ++) {
        functionButtons[pos].sprite.draw();
    }
}
