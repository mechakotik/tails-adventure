#ifndef TA_FONT_H
#define TA_FONT_H

#include <map>
#include <string>
#include "sprite.h"

class TA_Font : public TA_Sprite {
private:
    std::map<char, int> mapping;

public:
    void setMapping(std::string mappingString);
    void drawText(TA_Point position, std::string text, TA_Point offset = {0, 0});
    void drawTextCentered(double y, std::string text, TA_Point offset = {0, 0});
};

#endif // TA_FONT_H
