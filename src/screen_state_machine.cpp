#include "screen_state_machine.h"
#include "devmenu_screen.h"
#include "error.h"
#include "game_over_screen.h"
#include "game_screen.h"
#include "house_screen.h"
#include "intro_screen.h"
#include "main_menu_screen.h"
#include "map_screen.h"
#include "save.h"
#include "title_screen.h"

void TA_ScreenStateMachine::init() {
    if(TA::arguments.count("--devmenu")) {
        currentState = TA_SCREENSTATE_DEVMENU;
        currentScreen = std::make_unique<TA_DevmenuScreen>();
    } else {
        currentState = TA_SCREENSTATE_TITLE;
        currentScreen = std::make_unique<TA_TitleScreen>();
    }

    neededState = TA_SCREENSTATE_CURRENT;
    currentScreen->init();
}

bool TA_ScreenStateMachine::update() {
    TA_ScreenState returnedState = currentScreen->update();
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
        currentScreen->quit();
        TA::save::writeToFile();

        switch(neededState) {
            case TA_SCREENSTATE_INTRO:
                currentScreen = std::make_unique<TA_IntroScreen>();
                break;
            case TA_SCREENSTATE_TITLE:
                currentScreen = std::make_unique<TA_TitleScreen>();
                break;
            case TA_SCREENSTATE_GAME:
                currentScreen = std::make_unique<TA_GameScreen>();
                break;
            case TA_SCREENSTATE_DEVMENU:
                currentScreen = std::make_unique<TA_DevmenuScreen>();
                break;
            case TA_SCREENSTATE_MAP:
                currentScreen = std::make_unique<TA_MapScreen>();
                break;
            case TA_SCREENSTATE_HOUSE:
                currentScreen = std::make_unique<TA_HouseScreen>();
                break;
            case TA_SCREENSTATE_GAMEOVER:
                currentScreen = std::make_unique<TA_GameOverScreen>();
                break;
            case TA_SCREENSTATE_MAIN_MENU:
                currentScreen = std::make_unique<TA_MainMenuScreen>();
                break;
            default:
                TA::handleError("%s", "invalid new screen state");
                break;
        }

        currentScreen->init();
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

TA_ScreenStateMachine::~TA_ScreenStateMachine() {
    currentScreen->quit();
}
