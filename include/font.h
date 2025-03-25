#ifndef TA_FONT_H
#define TA_FONT_H

#include <filesystem>
#include <map>
#include <string>
#include "sprite.h"

class TA_Font : public TA_Sprite {
public:
    void loadFont(const std::filesystem::path& path);
    void setMapping(const std::string& mappingString);
    void drawText(TA_Point position, std::string text, TA_Point offset = {0, 0});
    void drawTextCentered(double y, std::string text, TA_Point offset = {0, 0});
    double getTextWidth(std::string text, TA_Point offset = {0, 0});

private:
    void tryLoadFont(const std::filesystem::path& path);
    static std::string getUtf8Char(const std::string& text, int pos);

    std::map<std::string, int> mapping;
};

#endif // TA_FONT_H
