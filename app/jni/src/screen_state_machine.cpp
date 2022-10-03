#include "screen_state_machine.h"
#include "intro_screen.h"
#include "title_screen.h"
#include "error.h"

void TA_ScreenStateMachine::init()
{
    currentState = TA_SCREENSTATE_INTRO;
    currentScreen = new TA_IntroScreen();
    currentScreen -> init();
}

bool TA_ScreenStateMachine::update()
{
    TA_ScreenState newState = currentScreen -> update();
    if(newState == TA_SCREENSTATE_CURRENT) {
        return false;
    }
    currentScreen -> quit();
    delete currentScreen;

    switch(newState)
    {
        case TA_SCREENSTATE_INTRO:
            currentScreen = new TA_IntroScreen();
            break;
        case TA_SCREENSTATE_TITLE:
            currentScreen = new TA_TitleScreen();
            break;
        default:
            handleError("%s", "Invalid new screen state");
            break;
    }

    currentScreen -> init();
    currentState = newState;
    return true;
}

TA_ScreenStateMachine::~TA_ScreenStateMachine()
{
    delete currentScreen;
}
