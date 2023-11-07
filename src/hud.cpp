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
    pauseMenuInactiveFont.load("fonts/pause_menu_inactive.png", 8, 8);
    pauseMenuInactiveFont.setMapping("abcdefghijklmnopqrstuvwxyz .*");
    pauseMenuFrameSprite.load("hud/pause_menu_frame.png");
    pauseMenuFrameSprite.setPosition(TA::screenWidth / 2 - 52, 42);

    itemPosition = TA::save::getSaveParameter("item_position");
    switchSound.load("sound/item_switch.ogg", TA_SOUND_CHANNEL_SFX1);
    flightBarSprite.load("hud/flightbar.png");
    rings = TA::save::getSaveParameter("rings");
    setPauseMenuAlpha(0);
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
        if(links.controller->isJustPressed(TA_BUTTON_PAUSE)) {
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

    if(links.controller->isJustChangedDirection()) {
        TA_Direction direction = links.controller->getDirection();
        if(direction == TA_DIRECTION_LEFT && itemPosition >= 1) {
            itemPosition --;
            TA::save::setSaveParameter("item_position", itemPosition);
        }
        else if(direction == TA_DIRECTION_RIGHT && itemPosition <= 2) {
            itemPosition ++;
            TA::save::setSaveParameter("item_position", itemPosition);
        }
        else if(direction == TA_DIRECTION_UP && pauseMenuSelection >= 1) {
            pauseMenuSelection --;
        }
        else if(direction == TA_DIRECTION_DOWN && pauseMenuSelection <= 1) {
            pauseMenuSelection ++;
        }
    }

    if(links.controller->isJustPressed(TA_BUTTON_PAUSE)) {
        exitPause = true;
        timer = 0;
    }
}

void TA_Hud::setHudAlpha(int alpha)
{
    ringMonitor.setAlpha(alpha);
    itemSprite.setAlpha(alpha);
    flightBarSprite.setAlpha(alpha);

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
    pauseMenuInactiveFont.setAlpha(alpha);
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
    int direction = 0;
    if(links.controller->isJustPressed(TA_BUTTON_LB)) {
        direction = -1;
    }
    else if(links.controller->isJustPressed(TA_BUTTON_RB)) {
        direction = 1;
    }
    if(direction != 0) {
        itemSprite.setAnimation(direction == 1 ? "item_switch_right" : "item_switch_left");
        itemPosition = (itemPosition + direction + 4) % 4;
        switchSound.play();
    }

    TA::save::setSaveParameter("item_position", itemPosition);
}

void TA_Hud::draw()
{
    drawCurrentItem();
    drawRingsCounter();
    drawFlightBar();
    drawPauseMenu();
}

void TA_Hud::drawCurrentItem()
{
    item = TA::save::getSaveParameter("item_slot" + std::to_string(itemPosition));
    if(item == -1) {
        item = 38;
    }

    if(!itemSprite.isAnimated()) {
        itemSprite.setFrame(item);
    }
    itemSprite.draw();
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
    double flightTime = links.character->getFlightTime();
    if(links.character->displayFlighTimeBar() && flightTime < 1) {
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
        int startX = TA::screenWidth / 2 - 41;

        for(int num = 0; num < 4; num ++) {
            pauseMenuItemSprite.setPosition(startX + num * 22, 48);

            int item = TA::save::getSaveParameter("item_slot" + std::to_string(num));
            pauseMenuItemSprite.setFrame(item);

            pauseMenuItemSprite.draw();
        }

        pointerSprite.setPosition(startX + itemPosition * 22 - 1, 46);
        pointerSprite.draw();
    }

    const std::array<std::string, 3> menu {
        "continue",
        "quit to map",
        "quit game"
    };

    for(int pos = 0; pos < (int)menu.size(); pos ++) {
        if(pos == pauseMenuSelection) {
            pauseMenuFont.drawTextCentered(70 + 10 * pos, menu[pos], {-1, 0});
        }
        else {
            pauseMenuInactiveFont.drawTextCentered(70 + 10 * pos, menu[pos], {-1, 0});
        }
    }
}
