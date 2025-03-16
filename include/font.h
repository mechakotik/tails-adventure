#ifndef TA_FONT_H
#define TA_FONT_H

#include <map>
#include <string>
#include <filesystem>
#include "sprite.h"

class TA_Font : public TA_Sprite {
public:
    void loadFont(std::filesystem::path path);
    void setMapping(std::string mappingString);
    void drawText(TA_Point position, std::string text, TA_Point offset = {0, 0});
    void drawTextCentered(double y, std::string text, TA_Point offset = {0, 0});
    double getTextWidth(std::string text, TA_Point offset = {0, 0});

private:
    void tryLoadFont(std::filesystem::path path);

    std::map<char, int> mapping;
};

#endif // TA_FONT_H
