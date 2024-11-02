#include <cmath>
#include "hud.h"
#include "character.h"
#include "save.h"
#include "screen.h"

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

    leftSprite.load("hud/items.png", 16, 16);
    leftSprite.setFrame(40);
    rightSprite.load("hud/items.png", 16, 16);
    rightSprite.setFrame(41);
    pauseSprite.load("hud/items.png", 16, 16);

    leftButton.setRectangle({0, 0}, {24, 24});
    rightButton.setRectangle({0, 0}, {24, 24});
    pauseButton.setRectangle({0, 0}, {16, 24});

    pauseMenu.load(links);

    itemPosition = TA::save::getSaveParameter("item_position");
    flightBarSprite.load("hud/flightbar.png");
    rings = TA::save::getSaveParameter("rings");

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
            pauseMenu.reset();
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
            pauseMenu.setAlpha(255 - 255 * timer / fadeTime);
        }
        else {
            setHudAlpha(255);
            timer = 0;
            paused = exitPause = false;
        }
        return;
    }

    if(timer < fadeTime) {
        setHudAlpha(255 - (255 * timer / fadeTime));
        pauseMenu.setAlpha(255 * timer / fadeTime);
        return;
    }

    setHudAlpha(0);
    pauseMenu.setAlpha(255);

    const TA_PauseMenu::UpdateResult result = pauseMenu.update();
    if(result == TA_PauseMenu::UpdateResult::RESUME) {
        exitPause = true;
        timer = 0;

        const std::string itemPositionKey = (links.seaFox == nullptr ? "item_position" : "seafox_item_position");
        itemPosition = static_cast<int>(TA::save::getSaveParameter(itemPositionKey));
    }
    if(result == TA_PauseMenu::UpdateResult::QUIT) {
        transition = TA_SCREENSTATE_MAP;
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
    if(paused) {
        pauseMenu.draw();
    }
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
