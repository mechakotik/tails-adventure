#ifndef TA_SCREENSTATEMACHINE_H
#define TA_SCREENSTATEMACHINE_H

#include "TA_Screen.h"

class TA_ScreenStateMachine {
private:
    TA_ScreenState currentState;
    TA_Screen *currentScreen;

public:
    TA_ScreenStateMachine();
    ~TA_ScreenStateMachine();
    void update();
};

#endif // TA_SCREENSTATEMACHINE_H
