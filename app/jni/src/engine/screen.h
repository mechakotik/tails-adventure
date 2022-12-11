#ifndef TA_SCREEN_H
#define TA_SCREEN_H

#include <utility>

enum TA_ScreenState
{
    TA_SCREENSTATE_CURRENT,
    TA_SCREENSTATE_INTRO,
    TA_SCREENSTATE_TITLE,
    TA_SCREENSTATE_GAME
};

class TA_Screen {
public:
    virtual void init() {}
    virtual TA_ScreenState update() {return TA_SCREENSTATE_CURRENT;}
    virtual void quit() {}
};

#endif // TA_SCREEN_H
