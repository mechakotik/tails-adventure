#include "font.h"

#include <utility>
#include "tools.h"
#include "error.h"
#include "resource_manager.h"

void TA_Font::loadFont(const std::filesystem::path& path) {
    try {
        tryLoadFont(path);
    } catch(std::exception &e) {
        TA::handleError("failed to load font:\n%s", e.what());
    }
}

void TA_Font::tryLoadFont(const std::filesystem::path& path) {
    toml::value table = TA::resmgr::loadToml(path);
    std::filesystem::path image = path.parent_path() / table.at("font").at("image").as_string();
    int width = static_cast<int>(table.at("font").at("width").as_integer());
    int height = static_cast<int>(table.at("font").at("height").as_integer());
    load(image, width, height);
    setMapping(table.at("font").at("mapping").as_string());
}

std::string TA_Font::getUtf8Char(const std::string& text, int pos) {
    if(pos >= text.size()) {
        TA::handleError("UTF8 symbol position out of range (text = %s, pos = %i)", text, pos);
    }

    unsigned char firstByte = static_cast<unsigned char>(text[pos]);
    int charLength = 0;
    if(firstByte <= 0x7FU) {
        charLength = 1;
    } else if((firstByte & 0xE0U) == 0xC0U) {
        if(firstByte < 0xC2U) {
            TA::handleError("invalid UTF8 first byte (text = %s, pos = %i)", text.c_str(), pos);
        }
        charLength = 2;
    } else if((firstByte & 0xF0U) == 0xE0U) {
        charLength = 3;
    } else if((firstByte & 0xF8U) == 0xF0U) {
        charLength = 4;
    } else {
        TA::handleError("invalid UTF8 first byte (text = %s, pos = %i)", text.c_str(), pos);
    }

    if(pos + charLength > text.size()) {
        TA::handleError("UTF8 symbol length out of range (text = %s, pos = %i, length = %i", text, pos, charLength);
    }
    return text.substr(pos, charLength);
}

void TA_Font::setMapping(const std::string& mappingString) {
    mapping.clear();
    int frame = 0;
    for(int pos = 0; pos < static_cast<int>(mappingString.length());) {
        std::string symbol = getUtf8Char(mappingString, pos);
        mapping[symbol] = frame;
        frame++;
        pos += static_cast<int>(symbol.size());
    }
}

void TA_Font::drawText(TA_Point position, std::string text, TA_Point offset) {
    TA_Point currentPosition = position;
    for(int pos = 0; pos < static_cast<int>(text.length());) {
        std::string symbol = getUtf8Char(text, pos);
        if(mapping.count(symbol)) {
            setPosition(currentPosition);
            setFrame(mapping[symbol]);
            draw();
            currentPosition.x += getWidth() + offset.x;
        } else if(symbol == "\n") {
            currentPosition.x = position.x;
            currentPosition.y += getHeight() + offset.y;
        }
        pos += static_cast<int>(symbol.size());
    }
}

void TA_Font::drawTextCentered(double y, std::string text, TA_Point offset) {
    double width = getTextWidth(text, offset);
    drawText(TA_Point(TA::screenWidth / 2 - width / 2, y), text, offset);
}

double TA_Font::getTextWidth(std::string text, TA_Point offset) {
    double currentWidth = 0;
    double maxWidth = 0;
    for(int pos = 0; pos < static_cast<int>(text.length());) {
        std::string symbol = getUtf8Char(text, pos);
        if(symbol != "\n") {
            currentWidth += getWidth() + offset.x;
            maxWidth = std::max(maxWidth, currentWidth);
        } else {
            currentWidth = 0;
        }
        pos += static_cast<int>(symbol.size());
    }
    return maxWidth;
}
