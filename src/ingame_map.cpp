#include "ingame_map.h"
#include "tools.h"

void TA_InGameMap::load() {
    double xOffset = (TA::screenWidth - 256) / 2;
    double yOffset = (TA::screenHeight - 144) / 2;
    tilemap.load("worldmap/map.tmx");
    tilemap.setPosition({xOffset, yOffset});

    namePlateSprite.load("worldmap/name_plate.png");
    namePlateSprite.setPosition(16, TA::screenHeight - 24);

    font.load("fonts/area.png", 8, 8);
    font.setMapping("abcdefghijklmnopqrstuvwxyz '.12");

    for(int pos = 0; pos < 2; pos++) {
        dolphinSprites[pos].loadFromToml("worldmap/dolphin.toml");
        dolphinSprites[pos].setAnimation("dolphin");
    }

    dolphinSprites[0].setPosition(76 + xOffset, yOffset + 24);
    dolphinSprites[1].setPosition(172 + xOffset, yOffset + 48);
}

void TA_InGameMap::draw() {
    drawBackground();
    tilemap.draw(0);
    for(int pos = 0; pos < 2; pos++) {
        dolphinSprites[pos].draw();
    }
}

void TA_InGameMap::drawBackground() {
    double yOffset = (TA::screenHeight - 144) / 2;
    TA::drawScreenRect(17, 136, 221, 255);
    TA::drawRect({0, 0}, {static_cast<double>(TA::screenWidth), yOffset + 9}, 187, 238, 238, 255);
}

void TA_InGameMap::drawSelectionName(std::string name) {
    namePlateSprite.draw();
    TA_Point textPosition = namePlateSprite.getPosition() + TA_Point(3, 4);
    TA_Point offset = {0, 0};
    if(font.getTextWidth(name) > namePlateSprite.getWidth() - 8) {
        offset = {-1, 0};
    }
    font.drawText(textPosition, name, offset);
}
