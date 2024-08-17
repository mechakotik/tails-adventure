#include "house_screen.h"
#include "tools.h"
#include "save.h"

void TA_HouseScreen::init()
{
    interfaceSprite.load("house/interface.png");
    houseSprite.load("house/house.png");
    curtainSprite.load("house/curtain.png");

    font.load("fonts/item.png", 8, 8);
    font.setMapping("abcdefghijklmnopqrstuvwxyz .*");
    controller.load();
    controller.setMode(TA_ONSCREEN_CONTROLLER_DPAD);

    TA::sound::playMusic("sound/house.vgm");
    switchSound.load("sound/switch.ogg", TA_SOUND_CHANNEL_SFX1);
    errorSound.load("sound/damage.ogg", TA_SOUND_CHANNEL_SFX2);
    selectSound.load("sound/select.ogg", TA_SOUND_CHANNEL_SFX2);

    inventoryMenu.load(&controller);
    optionsMenu.load(&controller);
}

TA_ScreenState TA_HouseScreen::update()
{
    controller.update();
    updatePositions();

    if(curtainTimeLeft <= 0) {
        if(inventoryOpen) {
            shouldMove |= !inventoryMenu.update();
            if(shouldMove) {
                inventoryMenu.hide();
            }
            else {
                inventoryMenu.show();
            }
        }
        else if(optionsOpen) {
            shouldMove |= !optionsMenu.update();
            if(shouldMove) {
                optionsMenu.hide();
            }
            else {
                optionsMenu.show();
            }
        }
        else {
            updateSelector();
        }
    }

    updateCurtain();
    draw();
    controller.draw();

    if(shouldExit) {
        TA::save::setSaveParameter("map_selection", 0);
        return TA_SCREENSTATE_MAP;
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_HouseScreen::updatePositions()
{
    double leftX = TA::screenWidth / 2 - interfaceSprite.getWidth() / 2;
    interfaceSprite.setPosition(leftX, 0);
    houseSprite.setPosition(leftX + 8, 32);
    curtainSprite.setPosition(leftX + 8, 32);
}

void TA_HouseScreen::updateSelector()
{
    if(!controller.isJustChangedDirection()) {
        return;
    }

    TA_Direction direction = controller.getDirection();
    std::pair<int, int> prevSelection{selectionX, selectionY};

    switch(direction) {
        case TA_DIRECTION_UP:
            selectionY = std::max(0, selectionY - 1);
            break;
        case TA_DIRECTION_DOWN:
            selectionY = std::min(1, selectionY + 1);
            break;
        case TA_DIRECTION_LEFT:
            selectionX = std::max(0, selectionX - 1);
            break;
        case TA_DIRECTION_RIGHT:
            selectionX = std::min(1, selectionX + 1);
            break;
        default:
            break;
    }

    if(prevSelection != std::make_pair(selectionX, selectionY)) {
        switchSound.play();
    }
}

void TA_HouseScreen::updateCurtain()
{
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

bool TA_HouseScreen::shouldDoTransition()
{
    if(inventoryOpen) {
        return !inventoryMenu.isShown();
    }
    if(optionsOpen) {
        return !optionsMenu.isShown();
    }
    if(!controller.isJustPressed(TA_BUTTON_A)) {
        return false;
    }

    if(selectionX == 0 && selectionY == 1) {
        errorSound.play();
        return false;
    }
    
    selectSound.play();
    return true;
}

void TA_HouseScreen::applyTransition()
{
    if(selectionX == 0 && selectionY == 0) {
        inventoryOpen = !inventoryOpen;
    }
    else if(selectionX == 1 && selectionY == 0) {
        optionsOpen = !optionsOpen;
        if(optionsOpen) {
            optionsMenu.reset();
        }
    }
    else if(selectionX == 1 && selectionY == 1) {
        shouldExit = true;
    }
}

void TA_HouseScreen::draw()
{
    TA::drawScreenRect(68, 56, 68, 255);
    interfaceSprite.draw();

    if(inventoryOpen) {
        inventoryMenu.draw();
    }
    else if(optionsOpen) {
        optionsMenu.draw();
    }
    else {
        houseSprite.draw();
    }

    drawSelector();
    drawCurtain();
}

void TA_HouseScreen::drawSelector()
{
    TA_Point position = interfaceSprite.getPosition() + TA_Point(32, 8);
    std::string text = ".equip.param\n.dock .exit";

    if(selectionX == 0) {
        if(selectionY == 0) {
            text[0] = '*';
        }
        else {
            text[13] = '*';
        }
    }
    else {
        if(selectionY == 0) {
            text[6] = '*';
        }
        else {
            text[19] = '*';
        }
    }

    font.drawText(position, text);
}

void TA_HouseScreen::drawCurtain()
{
    if(curtainTimeLeft < 0) {
        return;
    }
    if(shouldExit) {
        drawCurtain(1);
    }
    else if(curtainTimeLeft > curtainMoveTime + curtainIdleTime) {
        drawCurtain(1 - (curtainTimeLeft - curtainMoveTime - curtainIdleTime) / curtainMoveTime);
    }
    else if(curtainTimeLeft > curtainMoveTime) {
        drawCurtain(1);
    }
    else {
        drawCurtain(curtainTimeLeft / curtainMoveTime);
    }
}

void TA_HouseScreen::drawCurtain(double factor)
{
    SDL_Rect rect{0, 0, curtainSprite.getWidth(), int(curtainSprite.getHeight() * factor + 0.5)};
    curtainSprite.drawFrom(rect);
}
