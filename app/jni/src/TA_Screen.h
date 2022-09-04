#ifndef TA_SCREEN_H
#define TA_SCREEN_H

#include <utility>

enum TA_ScreenStateName
{
    TA_SCREENSTATE_CURRENT,
    TA_SCREENSTATE_INTRO
};

struct TA_ScreenState
{
    TA_ScreenStateName name;
    int param;
};

class TA_Screen {
public:
    virtual void init(int param = 0) {}
    virtual TA_ScreenState update() {return {TA_SCREENSTATE_CURRENT, 0};}
    virtual void quit() {}
};

#endif // TA_SCREEN_H
