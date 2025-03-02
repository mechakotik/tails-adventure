#ifndef TA_MAIN_MENU_SCREEN_H
#define TA_MAIN_MENU_SCREEN_H

#include <memory>
#include "main_menu_section.h"
#include "screen.h"

class TA_MainMenuScreen : public TA_Screen {
public:
    void init() override;
    TA_ScreenState update() override;

private:
    const double transitionTime = 5;

    void updateTitle();

    TA_Controller controller = TA_Controller();
    std::array<std::unique_ptr<TA_MainMenuSection>, TA_MAIN_MENU_MAX> sections;
    TA_MainMenuState state, neededState;
    TA_Sprite titleSprite;
    double timer = 0;
};

#endif // TA_MAIN_MENU_SCREEN_H
