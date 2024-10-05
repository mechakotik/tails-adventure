#include <cmath>
#include "hud.h"
#include "character.h"
#include "save.h"

void TA_Hud::load(TA_Links newLinks)
{
    links = newLinks;
    ringMonitor.load("hud/items.png", 16, 16);
    ringMonitor.loadAnimationsFromFile("hud/items.xml");
    ringMonitor.setAnimation("ring_monitor");
    ringMonitor.setPosition(2, 4);

    for(int pos = 0; pos < 2; pos ++) {
        ringDigits[pos].load("hud/numbers.png", 6, 11);
        ringDigits[pos].setPosition(20 + 6 * pos, 6);
    }

    itemSprite.load("hud/items.png", 16, 16);
    itemSprite.loadAnimationsFromFile("hud/items.xml");
    itemSprite.setPosition(2, 22);

    pauseMenuItemSprite.load("hud/items.png", 16, 16);
    pointerSprite.load("house/pointer.png");
    pauseMenuFont.load("fonts/pause_menu.png", 8, 8);
    pauseMenuFont.setMapping("abcdefghijklmnopqrstuvwxyz .*");
    pauseMenuFrameSprite.load("hud/pause_menu_frame.png");
    pauseMenuFrameSprite.setPosition(TA::screenWidth / 2 - pauseMenuFrameSprite.getWidth() / 2, 29);

    leftSprite.load("hud/items.png", 16, 16);
    leftSprite.setFrame(40);
    rightSprite.load("hud/items.png", 16, 16);
    rightSprite.setFrame(41);
    pauseSprite.load("hud/items.png", 16, 16);

    leftButton.setRectangle({0, 0}, {24, 24});
    rightButton.setRectangle({0, 0}, {24, 24});
    pauseButton.setRectangle({0, 0}, {16, 24});

    for(int pos = 0; pos < 4; pos ++) {
        itemButtons[pos].setRectangle({0, 0}, {20, 20});
    }
    for(int pos = 0; pos < 3; pos ++) {
        menuButtons[pos].setRectangle({0, 0}, {125, 17});
    }

    itemPosition = TA::save::getSaveParameter("item_position");
    flightBarSprite.load("hud/flightbar.png");
    rings = TA::save::getSaveParameter("rings");
    setPauseMenuAlpha(0);

    switchSound.load("sound/switch.ogg", TA_SOUND_CHANNEL_SFX1);
    itemSwitchSound.load("sound/item_switch.ogg", TA_SOUND_CHANNEL_SFX1);
    pauseSound.load("sound/enter.ogg", TA_SOUND_CHANNEL_SFX2);
}

void TA_Hud::update()
{
    updatePause();
    if(!paused) {
        updateRingsCounter();
        updateCurrentItem();
    }
}

void TA_Hud::updatePause()
{
    if(!paused) {
        pauseButton.setPosition(TA_Point(TA::screenWidth - 72, 0));
        pauseButton.update();
        pauseSprite.setFrame((pauseButton.isPressed() ? 43 : 42));
        if(links.controller->isJustPressed(TA_BUTTON_PAUSE) || pauseButton.isReleased()) {
            pauseSound.play();
            paused = true;
            timer = 0;
        }
        return;
    }

    updatePauseMenu();
}

void TA_Hud::updatePauseMenu()
{
    timer += TA::elapsedTime;

    if(exitPause) {
        if(timer < fadeTime) {
            setHudAlpha(255 * timer / fadeTime);
            setPauseMenuAlpha(255 - 255 * timer / fadeTime);
        }
        else {
            setHudAlpha(255);
            setPauseMenuAlpha(0);
            timer = 0;
            paused = exitPause = false;
        }
        return;
    }

    if(timer < fadeTime) {
        setHudAlpha(255 - 255 * timer / fadeTime);
        setPauseMenuAlpha(255 * timer / fadeTime);
        return;
    }
    else {
        setHudAlpha(0);
        setPauseMenuAlpha(255);
    }

    if(links.controller->isTouchscreen()) {
        updatePauseMenuInputTouch();
    }
    else {
        updatePauseMenuInputController();
    }
}

void TA_Hud::updatePauseMenuInputController()
{
    if(links.controller->isJustPressed(TA_BUTTON_PAUSE) ||
        links.controller->isJustPressed(TA_BUTTON_A) ||
        links.controller->isJustPressed(TA_BUTTON_B)) {
        pauseMenuSelect();
        return;
    }

    if(!links.controller->isJustChangedDirection()) {
        return;
    }

    TA_Direction direction = links.controller->getDirection();
    std::string itemPositionKey = (links.seaFox ? "seafox_item_position" : "item_position");

    if(direction == TA_DIRECTION_LEFT && itemPosition >= 1) {
        switchSound.play();
        itemPosition --;
        TA::save::setSaveParameter(itemPositionKey, itemPosition);
    }
    else if(direction == TA_DIRECTION_RIGHT && itemPosition <= 2) {
        switchSound.play();
        itemPosition ++;
        TA::save::setSaveParameter(itemPositionKey, itemPosition);
    }
    else if(direction == TA_DIRECTION_UP && pauseMenuSelection >= 1) {
        switchSound.play();
        pauseMenuSelection --;
    }
    else if(direction == TA_DIRECTION_DOWN && pauseMenuSelection <= 1) {
        switchSound.play();
        pauseMenuSelection ++;
    }
}

void TA_Hud::updatePauseMenuInputTouch()
{
    for(int pos = 0; pos < 3; pos ++) {
        menuButtons[pos].setPosition({TA::screenWidth / 2 - static_cast<double>(pauseMenuFrameSprite.getWidth() / 2), static_cast<double>(63 + 17 * pos)});
        menuButtons[pos].update();
        if(menuButtons[pos].isReleased()) {
            pauseMenuSelection = pos;
            pauseMenuSelect();
            return;
        }
    }

    std::string itemPositionKey = (links.seaFox ? "seafox_item_position" : "item_position");
    int startX = TA::screenWidth / 2 - 45;

    for(int pos = 0; pos < 4; pos ++) {
        itemButtons[pos].setPosition({static_cast<double>(startX + pos * 26), 36});
        itemButtons[pos].update();
        if(itemButtons[pos].isJustPressed()) {
            switchSound.play();
            itemPosition = pos;
            TA::save::setSaveParameter(itemPositionKey, itemPosition);
        }
    }
}

void TA_Hud::pauseMenuSelect()
{
    switch(pauseMenuSelection) {
        case 0: // continue
            pauseSound.play();
            exitPause = true;
            timer = 0;
            break;
        
        case 1: // quit to map
            transition = TA_SCREENSTATE_MAP;
            break;
        
        case 2: // quit game
            transition = TA_SCREENSTATE_QUIT;
            break;
    }
}

void TA_Hud::setHudAlpha(int alpha)
{
    links.controller->setAlpha(200 * alpha / 255);
    ringMonitor.setAlpha(alpha);
    flightBarSprite.setAlpha(alpha);

    itemSprite.setAlpha(alpha);
    leftSprite.setAlpha(alpha);
    rightSprite.setAlpha(alpha);
    pauseSprite.setAlpha(alpha);

    for(int digit = 0; digit < 2; digit ++) {
        ringDigits[digit].setAlpha(alpha);
    }
}

void TA_Hud::setPauseMenuAlpha(int alpha)
{
    pauseMenuItemSprite.setAlpha(alpha);
    pointerSprite.setAlpha(alpha);
    pauseMenuAlpha = alpha;
    pauseMenuFont.setAlpha(alpha);
    pauseMenuFrameSprite.setAlpha(alpha);
}

void TA_Hud::updateRingsCounter()
{
    int actualRings = TA::save::getSaveParameter("rings");
    actualRings = std::max(actualRings, 0);
    actualRings = std::min(actualRings, 99);

    timer += TA::elapsedTime;
    if(timer > ringAddTime) {
        if(rings < actualRings) {
            rings ++;
        }
        else if(rings > actualRings) {
            rings --;
        }
        timer = std::fmod(timer, ringAddTime);
    }
}

void TA_Hud::updateCurrentItem()
{
    leftButton.setPosition(TA_Point(TA::screenWidth - 52, 0));
    rightButton.setPosition(TA_Point(TA::screenWidth - 28, 0));
    leftButton.update();
    rightButton.update();

    int direction = 0;
    if(links.controller->isJustPressed(TA_BUTTON_LB) || leftButton.isJustPressed()) {
        direction = -1;
    }
    else if(links.controller->isJustPressed(TA_BUTTON_RB) || rightButton.isJustPressed()) {
        direction = 1;
    }
    if(direction != 0) {
        itemSprite.setAnimation(direction == 1 ? "item_switch_right" : "item_switch_left");
        itemPosition = (itemPosition + direction + 4) % 4;
        itemSwitchSound.play();
    }

    std::string itemPositionKey = (links.seaFox ? "seafox_item_position" : "item_position");
    TA::save::setSaveParameter(itemPositionKey, itemPosition);
}

void TA_Hud::draw()
{
    drawCurrentItem();
    drawRingsCounter();
    if(links.controller->isTouchscreen()) {
        drawTouchControls();
    }
    drawFlightBar();
    drawPauseMenu();
}

void TA_Hud::drawCurrentItem()
{
    std::string itemKey = (links.seaFox ? "seafox_item_slot" : "item_slot") + std::to_string(itemPosition);
    item = TA::save::getSaveParameter(itemKey);
    if(item == -1) {
        item = 38;
    }

    if(!itemSprite.isAnimated()) {
        itemSprite.setFrame(item);
    }

    if(links.controller->isTouchscreen()) {
        itemSprite.setPosition(TA::screenWidth - 36, 4);
    }
    else {
        itemSprite.setPosition(2, 22);
    }
    itemSprite.draw();
}

void TA_Hud::drawTouchControls()
{
    leftSprite.setPosition(TA::screenWidth - 52, 4);
    rightSprite.setPosition(TA::screenWidth - 20, 4);
    pauseSprite.setPosition(TA::screenWidth - 72, 4);

    leftSprite.draw();
    rightSprite.draw();
    pauseSprite.draw();
}

void TA_Hud::drawRingsCounter()
{
    ringMonitor.draw();
    if(rings >= 10) {
        ringDigits[0].setFrame(rings / 10);
        ringDigits[0].draw();
    }

    ringDigits[1].setFrame(rings % 10);
    ringDigits[1].draw();
}

void TA_Hud::drawFlightBar()
{
    if(!links.character) {
        return;
    }
    
    double flightTime = links.character->getFlightTime();
    if(links.character->displayFlightTimeBar() && flightTime < 1) {
        flightBarX = std::min(flightBarRight, flightBarX + flightBarSpeed * TA::elapsedTime);
    }
    else {
        flightBarX = std::max(flightBarLeft, flightBarX - flightBarSpeed * TA::elapsedTime);
    }

    int offset = 8 + std::min(24, int(24 * flightTime));
    flightBarSprite.setPosition(flightBarX, flightBarY);
    flightBarSprite.drawFrom({8, 0, 8, offset});
    flightBarSprite.setPosition(flightBarX, flightBarY + offset);
    flightBarSprite.drawFrom({0, offset, 8, 40 - offset});
}

void TA_Hud::drawPauseMenu()
{
    if(!paused) {
        return;
    }

    TA::drawScreenRect(0, 0, 0, pauseMenuAlpha / 2);
    pauseMenuFrameSprite.draw();

    {
        int startX = TA::screenWidth / 2 - 47;

        for(int num = 0; num < 4; num ++) {
            std::string itemKey = (links.seaFox ? "seafox_item_slot" : "item_slot") + std::to_string(num);
            int item = TA::save::getSaveParameter(itemKey);
            if(item == -1) {
                item = 38;
            }

            pauseMenuItemSprite.setPosition(startX + num * 26, 38);
            pauseMenuItemSprite.setFrame(item);
            pauseMenuItemSprite.draw();
        }

        pointerSprite.setPosition(startX + itemPosition * 26 - 1, 36);
        pointerSprite.draw();
    }

    const std::array<std::string, 3> menu {
        "continue",
        "quit to map",
        "quit game"
    };

    bool touchscreen = links.controller->isTouchscreen();
    for(int pos = 0; pos < (int)menu.size(); pos ++) {
        if((!touchscreen && pauseMenuSelection == pos) || (touchscreen && menuButtons[pos].isPressed())) {
            drawHighlight(60 + 17 * pos);
        }
        pauseMenuFont.drawTextCentered(63 + 17 * pos, menu[pos], {-1, 0});
    }
}

void TA_Hud::drawHighlight(double y)
{
    SDL_FRect rect = {(float)TA::screenWidth / 2 - 54, (float)y, 110, 15};

    for(int num = 0; num < 4; num ++) {
        int squareAlpha = static_cast<int>(255 * pauseMenuAlpha / 255 * pauseMenuAlpha / 255);
        SDL_SetRenderDrawColor(TA::renderer, num * 28, num * 24, num * 28, squareAlpha);

        SDL_FRect targetRect = rect;
        targetRect.x *= TA::scaleFactor;
        targetRect.y *= TA::scaleFactor;
        targetRect.w *= TA::scaleFactor;
        targetRect.h *= TA::scaleFactor;

        SDL_RenderFillRect(TA::renderer, &targetRect);
        rect.x += 2;
        rect.w -= 4;
    }
}
