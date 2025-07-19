#include "ingame_map.h"
#include "tools.h"

void TA_InGameMap::load() {
    float xOffset = (TA::screenWidth - 256) / 2;
    float yOffset = (TA::screenHeight - 144) / 2;

    mapSprite.loadFromToml("worldmap/map.toml");
    mapSprite.setPosition({xOffset, yOffset});
    mapSprite.setAnimation("map");

    namePlateSprite.load("worldmap/name_plate.png");
    namePlateSprite.setPosition(16, TA::screenHeight - 24);

    font.loadFont("fonts/area.toml");

    for(int i = 0; i < 2; i++) {
        dolphinSprites[i].loadFromToml("worldmap/dolphin.toml");
        dolphinSprites[i].setAnimation("dolphin");
    }

    dolphinSprites[0].setPosition(xOffset + 76, yOffset + 24);
    dolphinSprites[1].setPosition(xOffset + 172, yOffset + 48);

    for(int i = 0; i < 3; i++) {
        birdSprites[i].loadFromToml("worldmap/bird.toml");
        birdSprites[i].setAnimation("bird");
    }

    birdSprites[0].setPosition(xOffset + 68, yOffset + 88);
    birdSprites[1].setPosition(xOffset + 64, yOffset + 96);
    birdSprites[2].setPosition(xOffset + 72, yOffset + 96);
}

void TA_InGameMap::draw() {
    drawBackground();
    mapSprite.draw();
    for(int i = 0; i < 2; i++) {
        dolphinSprites[i].draw();
    }
    for(int i = 0; i < 3; i++) {
        birdSprites[i].draw();
    }
}

void TA_InGameMap::drawBackground() {
    float yOffset = (TA::screenHeight - 144) / 2;
    TA::drawScreenRect(17, 136, 221, 255);
    TA::drawRect({0, 0}, {static_cast<float>(TA::screenWidth), yOffset + 9}, 187, 238, 238, 255);
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
