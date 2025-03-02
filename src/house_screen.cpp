#include "house_screen.h"
#include "save.h"
#include "tools.h"

void TA_HouseScreen::init() {
    interfaceSprite.load("house/interface.png");
    houseSprite.load("house/house.png");
    houseSeaFoxSprite.load("house/house_seafox.png");
    clawSprite.load("house/claw.png");
    seaFoxSprite.load("house/seafox.png");
    curtainSprite.load("house/curtain.png");
    selectorSprite.load("house/selector.png", 16, 12);
    controller.load();
    TA::sound::playMusic("sound/house.vgm");
    switchSound.load("sound/switch.ogg", TA_SOUND_CHANNEL_SFX1);
    errorSound.load("sound/damage.ogg", TA_SOUND_CHANNEL_SFX2);
    selectSound.load("sound/select.ogg", TA_SOUND_CHANNEL_SFX2);

    for(int pos = 0; pos < 4; pos++) {
        buttons[pos].setRectangle({0, 0}, {23, 32});
    }

    inventoryMenu.load(&controller);
}

TA_ScreenState TA_HouseScreen::update() {
    controller.update();
    updatePositions();

    if(curtainTimeLeft <= 0) {
        if(inventoryOpen) {
            if(controller.isTouchscreen()) {
                updateSelectorTouch();
                if(buttons[3].isReleased()) {
                    selectSound.play();
                    shouldMove = true;
                }
            }
            shouldMove |= !inventoryMenu.update();
            if(shouldMove) {
                inventoryMenu.hide();
            } else {
                inventoryMenu.show();
            }
        } else {
            updateSelector();
        }
    }

    updateCurtain();
    draw();
    controller.draw();

    if(shouldExit) {
        TA::save::setSaveParameter("map_selection", 0);
        return (selection == 2 ? TA_SCREENSTATE_MAIN_MENU : TA_SCREENSTATE_MAP);
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_HouseScreen::updatePositions() {
    double leftX = TA::screenWidth / 2 - interfaceSprite.getWidth() / 2;
    interfaceSprite.setPosition(leftX, 0);
    houseSprite.setPosition(leftX + 8, 32);
    houseSeaFoxSprite.setPosition(leftX + 8, 32);
    curtainSprite.setPosition(leftX + 8, 32);

    if(clawDirection) {
        clawX += TA::elapsedTime;
        if(clawX > 106) {
            clawX = 106;
            clawDirection = false;
        }
    } else {
        clawX -= TA::elapsedTime;
        if(clawX < 40) {
            clawX = 40;
            clawDirection = true;
        }
    }

    clawSprite.setPosition(leftX + clawX, 32);
    seaFoxSprite.setPosition(leftX + 65, 45);
}

void TA_HouseScreen::updateSelector() {
    if(controller.isTouchscreen()) {
        updateSelectorTouch();
    } else {
        updateSelectorController();
    }
}

void TA_HouseScreen::updateSelectorController() {
    if(!controller.isJustChangedDirection()) {
        return;
    }
    TA_Direction direction = controller.getDirection();
    if(direction == TA_DIRECTION_LEFT && selection - 1 >= 0) {
        selection--;
        switchSound.play();
    } else if(direction == TA_DIRECTION_RIGHT && selection + 1 < 4) {
        selection++;
        switchSound.play();
    }
}

void TA_HouseScreen::updateSelectorTouch() {
    double leftX = TA::screenWidth / 2 - interfaceSprite.getWidth() / 2;
    for(int pos = 0; pos < 4; pos++) {
        buttons[pos].setPosition({leftX + 31 + 23 * pos, 0});
        buttons[pos].update();
        if(buttons[pos].isReleased() && !inventoryOpen) {
            selection = pos;
        }
    }
}

void TA_HouseScreen::updateCurtain() {
    if(curtainTimeLeft <= 0) {
        if(shouldDoTransition()) {
            curtainTimeLeft = curtainMoveTime * 2 + curtainIdleTime;
            shouldMove = false;
        }
        return;
    }

    double prev = curtainTimeLeft;
    curtainTimeLeft -= TA::elapsedTime;

    if(prev > curtainMoveTime && curtainTimeLeft <= curtainMoveTime) {
        applyTransition();
    }
}

bool TA_HouseScreen::shouldDoTransition() {
    if(inventoryOpen) {
        return !inventoryMenu.isShown();
    }

    bool touchscreen = controller.isTouchscreen();
    if(!touchscreen && !controller.isJustPressed(TA_BUTTON_A)) {
        return false;
    }
    if(touchscreen) {
        bool pressed = false;
        for(int pos = 0; pos < 4; pos++) {
            if(buttons[pos].isReleased()) {
                pressed = true;
            }
        }
        if(!pressed) {
            return false;
        }
    }

    if(selection == 1 && !isSeaFoxAvailable()) {
        errorSound.play();
        return false;
    }

    selectSound.play();
    return true;
}

bool TA_HouseScreen::isSeaFoxAvailable() {
    long long itemMask = TA::save::getSaveParameter("item_mask");
    return itemMask & (1ll << 33);
}

void TA_HouseScreen::applyTransition() {
    if(selection == 0) {
        inventoryOpen = !inventoryOpen;
    } else if(selection == 1) {
        bool seaFox = TA::save::getSaveParameter("seafox");
        seaFox = !seaFox;
        TA::save::setSaveParameter("seafox", seaFox);
    } else {
        shouldExit = true;
    }
}

void TA_HouseScreen::draw() {
    TA::drawScreenRect(68, 56, 68, 255);
    interfaceSprite.draw();

    if(inventoryOpen) {
        inventoryMenu.draw();
    } else {
        if(TA::save::getSaveParameter("seafox")) {
            houseSeaFoxSprite.draw();
            clawSprite.draw();
            seaFoxSprite.draw();
        } else {
            houseSprite.draw();
        }
    }

    drawSelector();
    drawCurtain();
}

void TA_HouseScreen::drawSelector() {
    double leftX = TA::screenWidth / 2 - interfaceSprite.getWidth() / 2;
    bool touchscreen = controller.isTouchscreen();

    for(int pos = 0; pos < 4; pos++) {
        int frame = pos;
        if(pos > 1 || (pos == 1 && TA::save::getSaveParameter("seafox"))) {
            frame++;
        }

        if(inventoryOpen) {
            if(pos == 3 && touchscreen) {
                if(buttons[pos].isPressed()) {
                    frame += 5;
                }
            } else {
                frame += 10;
            }
        } else if((!touchscreen && selection == pos) || (touchscreen && buttons[pos].isPressed())) {
            frame += 5;
        }

        selectorSprite.setPosition(leftX + 37 + pos * 23, 10);
        selectorSprite.setFrame(frame);
        selectorSprite.draw();
    }
}

void TA_HouseScreen::drawCurtain() {
    if(curtainTimeLeft < 0) {
        return;
    }
    if(shouldExit) {
        drawCurtain(1);
    } else if(curtainTimeLeft > curtainMoveTime + curtainIdleTime) {
        drawCurtain(1 - (curtainTimeLeft - curtainMoveTime - curtainIdleTime) / curtainMoveTime);
    } else if(curtainTimeLeft > curtainMoveTime) {
        drawCurtain(1);
    } else {
        drawCurtain(curtainTimeLeft / curtainMoveTime);
    }
}

void TA_HouseScreen::drawCurtain(double factor) {
    SDL_Rect rect{0, 0, curtainSprite.getWidth(), int(curtainSprite.getHeight() * factor + 0.5)};
    curtainSprite.drawFrom(rect);
}
