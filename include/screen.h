#ifndef TA_SCREEN_H
#define TA_SCREEN_H

#include <utility>

enum TA_ScreenState
{
    TA_SCREENSTATE_CURRENT,
    TA_SCREENSTATE_INTRO,
    TA_SCREENSTATE_TITLE,
    TA_SCREENSTATE_GAME,
    TA_SCREENSTATE_DEVMENU,
    TA_SCREENSTATE_MAP,
    TA_SCREENSTATE_HOUSE,
    TA_SCREENSTATE_GAMEOVER,
    TA_SCREENSTATE_MAIN_MENU,
    TA_SCREENSTATE_QUIT
};

class TA_Screen {
public:
    virtual void init() {}
    virtual TA_ScreenState update() {return TA_SCREENSTATE_CURRENT;}
    virtual void quit() {} // TODO: is this really needed?
    virtual ~TA_Screen() = default;
};

#endif // TA_SCREEN_H
