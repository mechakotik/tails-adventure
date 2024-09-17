#ifndef TA_MAIN_MENU_SCREEN_H
#define TA_MAIN_MENU_SCREEN_H

#include <memory>
#include "screen.h"
#include "main_menu_section.h"

class TA_MainMenuScreen : public TA_Screen {
private:
    TA_Controller controller = TA_Controller();
    std::array<std::unique_ptr<TA_MainMenuSection>, TA_MAIN_MENU_MAX> sections;
    TA_MainMenuState state, neededState;

public:
    void init() override;
    TA_ScreenState update() override;
};

#endif // TA_MAIN_MENU_SCREEN_H
