#include "font.h"

#include <utility>
#include "tools.h"
#include "error.h"
#include "resource_manager.h"

void TA_Font::loadFont(std::filesystem::path path) {
    try {
        tryLoadFont(std::move(path));
    } catch(std::exception &e) {
        TA::handleError("Failed to load font:\n%s", e.what());
    }
}

void TA_Font::tryLoadFont(std::filesystem::path path) {
    toml::value table = TA::resmgr::loadToml(path);
    std::filesystem::path image = path.parent_path() / table.at("font").at("image").as_string();
    int width = static_cast<int>(table.at("font").at("width").as_integer());
    int height = static_cast<int>(table.at("font").at("height").as_integer());
    load(image, width, height);
    setMapping(table.at("font").at("mapping").as_string());
}

void TA_Font::setMapping(std::string mappingString) {
    // TODO: proper UTF8 mapping support
    mapping.clear();
    for(int pos = 0; pos < static_cast<int>(mappingString.length()); pos++) {
        mapping[mappingString[pos]] = pos;
    }
}

void TA_Font::drawText(TA_Point position, std::string text, TA_Point offset) {
    TA_Point currentPosition = position;
    for(int pos = 0; pos < static_cast<int>(text.length()); pos++) {
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
