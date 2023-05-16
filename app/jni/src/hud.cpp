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
}

void TA_Hud::update()
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
    }
    TA::save::setSaveParameter("item_position", itemPosition);
    item = TA::save::getSaveParameter("item_slot" + std::to_string(itemPosition));
    if(item == -1) {
        item = 38;
    }
}

void TA_Hud::draw()
{
    if(!itemSprite.isAnimated()) {
        itemSprite.setFrame(item);
    }
    itemSprite.draw();

    ringMonitor.draw();
    int rings = links.character->getRingsCount();
    rings = std::min(rings, 99);
    if(rings >= 10) {
        ringDigits[0].setFrame(rings / 10);
        ringDigits[0].draw();
    }
    ringDigits[1].setFrame(rings % 10);
    ringDigits[1].draw();
}
