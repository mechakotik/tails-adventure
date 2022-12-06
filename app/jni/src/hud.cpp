#include "hud.h"
#include "character.h"

void TA_Hud::load(TA_GameScreenLinks newLinks)
{
    links = newLinks;
    ringMonitor.load("hud/items.png", 16, 16);
    ringMonitor.loadAnimationsFromFile("hud/animations.xml");
    ringMonitor.setAnimation("ring_monitor");
    ringMonitor.setPosition(2, 4);

    for(int pos = 0; pos < 2; pos ++) {
        ringDigits[pos].load("hud/numbers.png", 6, 11);
        ringDigits[pos].setPosition(20 + 6 * pos, 6);
    }

    currentItem.load("hud/items.png", 16, 16);
    currentItem.setPosition(2, 22);
}

void TA_Hud::draw()
{
    int item = links.character->getCurrentItem();
    currentItem.setFrame(item);
    currentItem.draw();

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
