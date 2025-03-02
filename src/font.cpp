#include "font.h"
#include "tools.h"

void TA_Font::setMapping(std::string mappingString) {
    mapping.clear();
    for(int pos = 0; pos < (int)mappingString.length(); pos++) {
        mapping[mappingString[pos]] = pos;
    }
}

void TA_Font::drawText(TA_Point position, std::string text, TA_Point offset) {
    TA_Point currentPosition = position;
    for(int pos = 0; pos < (int)text.length(); pos++) {
        char symbol = text[pos];
        if(mapping.count(symbol)) {
            setPosition(currentPosition);
            setFrame(mapping[symbol]);
            draw();
            currentPosition.x += getWidth() + offset.x;
        } else if(symbol == '\n') {
            currentPosition.x = position.x;
            currentPosition.y += getHeight() + offset.y;
        }
    }
}

void TA_Font::drawTextCentered(double y, std::string text, TA_Point offset) {
    double width = getTextWidth(text, offset);
    drawText(TA_Point(TA::screenWidth / 2 - width / 2, y), text, offset);
}

double TA_Font::getTextWidth(std::string text, TA_Point offset) {
    double currentWidth = 0, maxWidth = 0;
    for(char symbol : text) {
        if(symbol != '\n') {
            currentWidth += getWidth() + offset.x;
            maxWidth = std::max(maxWidth, currentWidth);
        } else {
            currentWidth = 0;
        }
    }
    return maxWidth;
}
