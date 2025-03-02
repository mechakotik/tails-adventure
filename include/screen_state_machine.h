#ifndef TA_SCREEN_STATE_MACHINE_H
#define TA_SCREEN_STATE_MACHINE_H

#include <memory>
#include "screen.h"

class TA_ScreenStateMachine {
private:
    TA_ScreenState currentState, neededState;
    std::unique_ptr<TA_Screen> currentScreen;
    double transitionTimer = 0;
    bool changeState = false, quitNeeded = false;

    const double transitionTime = 6;

public:
    void init();
    bool update();
    bool isQuitNeeded() { return quitNeeded; }
    ~TA_ScreenStateMachine();
};

#endif // TA_SCREEN_STATE_MACHINE_H
