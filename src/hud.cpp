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
    itemPosition = TA::save::getSaveParameter("item_position");
    switchSound.load("sound/item_switch.ogg", TA_SOUND_CHANNEL_SFX1);
    flightBarSprite.load("hud/flightbar.png");
    rings = TA::save::getSaveParameter("rings");
}

void TA_Hud::update()
{
    updateRingsCounter();
    updateCurrentItem();
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
    item = TA::save::getSaveParameter("item_slot" + std::to_string(itemPosition));
    if(item == -1) {
        item = 38;
    }
}

void TA_Hud::draw()
{
    drawCurrentItem();
    drawRingsCounter();
    drawFlightBar();
}

void TA_Hud::drawCurrentItem()
{
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
