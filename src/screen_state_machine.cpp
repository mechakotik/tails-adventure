#include "screen_state_machine.h"
#include "intro_screen.h"
#include "title_screen.h"
#include "game_screen.h"
#include "devmenu_screen.h"
#include "map_screen.h"
#include "house_screen.h"
#include "game_over_screen.h"
#include "error.h"
#include "save.h"

void TA_ScreenStateMachine::init()
{
    currentState = TA_SCREENSTATE_DEVMENU;
    neededState = TA_SCREENSTATE_CURRENT;
    currentScreen = new TA_DevmenuScreen();
    currentScreen -> init();
}

bool TA_ScreenStateMachine::update()
{
    TA_ScreenState returnedState = currentScreen -> update();
    if(returnedState == TA_SCREENSTATE_QUIT) {
        returnedState = TA_SCREENSTATE_CURRENT;
        quitNeeded = true;
    }

    if(neededState == TA_SCREENSTATE_CURRENT && returnedState != TA_SCREENSTATE_CURRENT) {
        neededState = returnedState;
        TA::sound::fadeOut(transitionTime + 2);
    }
    if(neededState == TA_SCREENSTATE_CURRENT) {
        if(transitionTimer > 0) {
            TA::drawShadow(255 * transitionTimer / transitionTime);
            transitionTimer -= TA::elapsedTime;
        }
        return false;
    }

    if(changeState) {
        TA::drawShadow(255);
        currentScreen -> quit();
        delete currentScreen;

        TA::save::writeToFile();

        switch(neededState)
        {
            case TA_SCREENSTATE_INTRO:
                currentScreen = new TA_IntroScreen();
                break;
            case TA_SCREENSTATE_TITLE:
                currentScreen = new TA_TitleScreen();
                break;
            case TA_SCREENSTATE_GAME:
                currentScreen = new TA_GameScreen();
                break;
            case TA_SCREENSTATE_DEVMENU:
                currentScreen = new TA_DevmenuScreen();
                break;
            case TA_SCREENSTATE_MAP:
                currentScreen = new TA_MapScreen();
                break;
            case TA_SCREENSTATE_HOUSE:
                currentScreen = new TA_HouseScreen();
                break;
            case TA_SCREENSTATE_GAMEOVER:
                currentScreen = new TA_GameOverScreen();
                break;
            default:
                TA::handleError("%s", "Invalid new screen state");
                break;
        }

        currentScreen -> init();
        currentState = neededState;
        neededState = TA_SCREENSTATE_CURRENT;
        changeState = false;
        return true;
    }

    transitionTimer = std::max(double(0), transitionTimer) + TA::elapsedTime;
    TA::drawShadow(255 * transitionTimer / transitionTime);
    if(transitionTimer > transitionTime) {
        changeState = true;
    }
    return false;
}

TA_ScreenStateMachine::~TA_ScreenStateMachine()
{
    currentScreen->quit();
    delete currentScreen;
}
