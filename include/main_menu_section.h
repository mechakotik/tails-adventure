#ifndef TA_MAIN_MENU_SECTION_H
#define TA_MAIN_MENU_SECTION_H

#include "controller.h"

enum TA_MainMenuState {
    TA_MAIN_MENU_DATA_SELECT,
    TA_MAIN_MENU_OPTIONS,
    TA_MAIN_MENU_EXIT,
    TA_MAIN_MENU_MAX
};

class TA_MainMenuSection {
public:
    TA_MainMenuSection(TA_Controller *controller) {this->controller = controller;}
    virtual void load() {}
    virtual TA_MainMenuState update() {return TA_MAIN_MENU_EXIT;}
    virtual void setAlpha(int alpha) {}
    virtual void draw() {}
    virtual void reset() {}
    virtual ~TA_MainMenuSection() = default;

protected:
    TA_Controller *controller;
};

#endif // TA_MAIN_MENU_SECTION_H
