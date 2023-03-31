#ifndef TA_SCREENSTATEMACHINE_H
#define TA_SCREENSTATEMACHINE_H

#include "screen.h"

class TA_ScreenStateMachine {
private:
    TA_ScreenState currentState, neededState;
    TA_Screen *currentScreen;
    double transitionTimer = 0;
    bool changeState = false;

    const double transitionTime = 6;

public:
    void init();
    bool update();
    ~TA_ScreenStateMachine();
};

#endif // TA_SCREENSTATEMACHINE_H
