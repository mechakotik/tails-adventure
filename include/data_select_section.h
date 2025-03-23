#ifndef DATA_SELECT_SECTION_H
#define DATA_SELECT_SECTION_H

#include "font.h"
#include "main_menu_section.h"
#include "sound.h"

class TA_DataSelectSection : public TA_MainMenuSection {
public:
    using TA_MainMenuSection::TA_MainMenuSection;
    void load() override;
    TA_MainMenuState update() override;
    void setAlpha(int alpha) override { this->alpha = alpha; }
    void draw() override;

private:
    const double menuStart = 16;
    const double menuOffset = 64;
    const double scrollSpeed = 12;
    const double selectorBlinkTime = 15;
    const double loadTime = 60;
    const double scrollSlowdown = 0.125;

    void updateScroll();
    void updateSelection();
    bool updateTouchscreenSelection();
    int getSavePercent(int save);
    std::string getSaveTime(int save);
    TA_MainMenuState processSelection();
    void drawCustomEntries();
    void drawSaveEntries();
    void drawModCount();
    void drawSplash();
    void drawSelector();
    std::string generateSplash();

    TA_Sprite entrySprite, selectorRedSprite, selectorWhiteSprite, previewSprite, optionsSprite;
    TA_Sound switchSound, selectSound, loadSaveSound;
    TA_Font font, splashFont;
    std::string splash;
    double timer = 0, selectorTimer = 0;

    std::array<TA_OnscreenButton, 9> buttons;
    double position = 0, scrollVelocity = 0;
    int selection = 1, createdSave = -1, alpha = 255;
    bool locked = false;
};

#endif // DATA_SELECT_SECTION_H
