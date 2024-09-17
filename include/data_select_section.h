#ifndef DATA_SELECT_SECTION_H
#define DATA_SELECT_SECTION_H

#include "main_menu_section.h"

class TA_DataSelectSection : public TA_MainMenuSection {
public:
    using TA_MainMenuSection::TA_MainMenuSection;
    void load() override;
    TA_MainMenuState update() override;
    void setAlpha(int alpha) override;
    void draw() override;

private:
    const double menuStart = 16;
    const double menuOffset = 64;

    TA_Sprite entrySprite;
    double position = 0;
    int selection = 0;
};

#endif // DATA_SELECT_SECTION_H
