#include "TA_ScreenStateMachine.h"
#include "TA_IntroScreen.h"
#include "TA_Error.h"

TA_ScreenStateMachine::TA_ScreenStateMachine()
{
    currentState.name = TA_SCREENSTATE_INTRO;
    currentState.param = 0;
    currentScreen = new TA_IntroScreen();
    currentScreen -> init();
}

void TA_ScreenStateMachine::update()
{
    TA_ScreenState newState = currentScreen -> update();
    if(newState.name == TA_SCREENSTATE_CURRENT) {
        return;
    }
    currentScreen -> quit();
    delete currentScreen;

    switch(newState.name)
    {
        case TA_SCREENSTATE_INTRO:
            currentScreen = new TA_IntroScreen();
            break;
        default:
            handleError("Invalid new screen state");
            break;
    }

    currentScreen -> init(newState.param);
    currentState = newState;
}

TA_ScreenStateMachine::~TA_ScreenStateMachine()
{
    delete currentScreen;
}
