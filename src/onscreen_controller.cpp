#include "controller.h"

void TA_OnscreenController::load()
{
    sprites[TA_BUTTON_A].load("controls/a_button.png", 20, 22);
    sprites[TA_BUTTON_B].load("controls/b_button.png", 20, 22);
    sprites[TA_BUTTON_A].setAlpha(200);
    sprites[TA_BUTTON_B].setAlpha(200);

    arrowSprites[TA_DIRECTION_UP].load("controls/up_button.png", 18, 20);
    arrowSprites[TA_DIRECTION_DOWN].load("controls/down_button.png", 18, 20);
    arrowSprites[TA_DIRECTION_LEFT].load("controls/left_button.png", 18, 20);
    arrowSprites[TA_DIRECTION_RIGHT].load("controls/right_button.png", 18, 20);

    stickSprite.load("controls/stick.png");
    pointerSprite.load("controls/pointer.png");
    stickSprite.setAlpha(200);
    pointerSprite.setAlpha(200);
    updatePositions();
}

void TA_OnscreenController::update()
{
    if(mode == TA_ONSCREEN_CONTROLLER_DISABLED) {
        return;
    }

    updatePositions();

    if(mode == TA_ONSCREEN_CONTROLLER_DPAD) {
        for(int pos = 0; pos < TA_DIRECTION_MAX; pos ++) {
            arrowButtons[pos].update();
        }
    }
    else {
        updateStick();
    }

    for(int pos = 0; pos < TA_BUTTON_MAX; pos ++) {
        buttons[pos].update();
    }
}

void TA_OnscreenController::draw()
{
    if(mode == TA_ONSCREEN_CONTROLLER_DISABLED) {
        return;
    }

    if(mode == TA_ONSCREEN_CONTROLLER_DPAD) {
        for(int pos = 0; pos < TA_DIRECTION_MAX; pos ++) {
            arrowSprites[pos].setFrame(arrowButtons[pos].isPressed() ? 1 : 0);
            arrowSprites[pos].draw();
        }
    }
    else {
        stickSprite.draw();
        pointerSprite.draw();
    }

    for(int pos = 0; pos < TA_BUTTON_MAX; pos ++) {
        sprites[pos].setFrame(buttons[pos].isPressed() ? 1 : 0);
        sprites[pos].draw();
    }
}

void TA_OnscreenController::updateStick()
{
    stick.update();

    if(stick.isPressed()) {
        vector = stick.getTouchPosition() - stick.getVertex(0);
        vector.x /= stickSprite.getWidth() / 2;
        vector.y /= stickSprite.getHeight() / 2;
        if(vector.length() > 1) {
            vector.x /= vector.length();
            vector.y /= vector.length();
        }
    }
    else {
        vector = {0, 0};
    }

    TA_Point position = vector * TA_Point(stickSprite.getWidth() / 2, stickSprite.getHeight() / 2);
    position = position + stickSprite.getPosition();
    position.x += (stickSprite.getWidth() - pointerSprite.getWidth()) / 2;
    position.y += (stickSprite.getHeight() - pointerSprite.getHeight()) / 2;
    pointerSprite.setPosition(position);
}

void TA_OnscreenController::updatePositions()
{
    setButtonPosition(TA_BUTTON_A, TA_Point(TA::screenWidth - 35, TA::screenHeight - 25));
    setButtonPosition(TA_BUTTON_B, TA_Point(TA::screenWidth - 25, TA::screenHeight - 50));

    if(mode == TA_ONSCREEN_CONTROLLER_DPAD) {
        setArrowButtonPosition(TA_DIRECTION_UP, TA_Point(40, TA::screenHeight - 55));
        setArrowButtonPosition(TA_DIRECTION_DOWN, TA_Point(40, TA::screenHeight - 25));
        setArrowButtonPosition(TA_DIRECTION_LEFT, TA_Point(25, TA::screenHeight - 40));
        setArrowButtonPosition(TA_DIRECTION_RIGHT, TA_Point(55, TA::screenHeight - 40));
    }
    else {
        TA_Point center = TA_Point(35, TA::screenHeight - 37);
        stickSprite.setPosition(center - TA_Point(stickSprite.getWidth() / 2, stickSprite.getHeight() / 2));
        stick.setCircle(center, 60);
    }
}

void TA_OnscreenController::setButtonPosition(TA_FunctionButton button, TA_Point center)
{
    buttons[button].setCircle(center, 12);
    sprites[button].setPosition(center - TA_Point(sprites[button].getWidth() / 2, sprites[button].getHeight() / 2));
}

void TA_OnscreenController::setArrowButtonPosition(TA_Direction button, TA_Point center)
{
    arrowButtons[button].setCircle(center, 12);
    arrowSprites[button].setPosition(center - TA_Point(arrowSprites[button].getWidth() / 2, arrowSprites[button].getHeight() / 2));
}

TA_Point TA_OnscreenController::getDirectionVector()
{
    if(mode != TA_ONSCREEN_CONTROLLER_DPAD) {
        return vector;
    }

    if(arrowButtons[TA_DIRECTION_UP].isPressed()) {
        return {0, -1};
    }
    if(arrowButtons[TA_DIRECTION_DOWN].isPressed()) {
        return {0, 1};
    }
    if(arrowButtons[TA_DIRECTION_LEFT].isPressed()) {
        return {-1, 0};
    }
    if(arrowButtons[TA_DIRECTION_RIGHT].isPressed()) {
        return {1, 0};
    }
    return {0, 0};
}

bool TA_OnscreenController::isPressed(TA_FunctionButton button)
{
    return buttons[button].isPressed();
}

bool TA_OnscreenController::isJustPressed(TA_FunctionButton button)
{
    return buttons[button].isJustPressed();
}
