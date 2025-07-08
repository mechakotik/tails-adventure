#ifndef TA_DEVMENU_SCREEN_H
#define TA_DEVMENU_SCREEN_H

#include "controller.h"
#include "font.h"
#include "screen.h"

enum TA_DevmenuElements {
    TA_DEVMENU_ELEMENT_LEVEL,
    TA_DEVMENU_ELEMENT_MAX,
};

class TA_DevmenuScreen : public TA_Screen {
private:
    TA_Font normalFont, selectedFont;
    TA_Controller controller;
    int menuPosition = 0, levelPosition = 0;

    std::vector<std::string> levels;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override;
};

#endif // TA_DEVMENU_SCREEN_H
