#ifndef TA_SCREENSTATEMACHINE_H
#define TA_SCREENSTATEMACHINE_H

#include "screen.h"

class TA_ScreenStateMachine {
private:
    TA_ScreenState currentState;
    TA_Screen *currentScreen;

public:
    void init();
    bool update();
    ~TA_ScreenStateMachine();
};

#endif // TA_SCREENSTATEMACHINE_H
