#ifndef TA_OPTIONS_MENU_H
#define TA_OPTIONS_MENU_H

#include <memory>
#include "controller.h"
#include "font.h"
#include "sound.h"

enum TA_MoveSoundId {
    TA_MOVE_SOUND_SWITCH,
    TA_MOVE_SOUND_SELECT,
    TA_MOVE_SOUND_ERROR,
    TA_MOVE_SOUND_EMPTY
};

class TA_Option {
public:
    virtual std::string getName() {return "name";}
    virtual std::string getValue() {return "value";}
    virtual TA_MoveSoundId move(int delta) {return TA_MOVE_SOUND_ERROR;}

    virtual bool isLocked() {return false;}
    virtual TA_MoveSoundId updateLocked() {return TA_MOVE_SOUND_EMPTY;}
};

class TA_OptionsMenu {
private:
    const double transitionTime = 10;
    const double listTransitionTime = 5;

    void updateGroupSelector();
    void updateOptionSelector();
    void playMoveSound(TA_MoveSoundId id);

    void updateAlpha();
    int getLeftX() {return TA::screenWidth / 2 - 80;}

    void drawGroupList();
    void drawOptionList();

    enum State {
        STATE_SELECTING_GROUP,
        STATE_SELECTING_OPTION,
        STATE_QUIT
    };

    TA_Controller* controller;
    TA_Font activeFont, inactiveFont;
    double transitionTimeLeft = -1, listTransitionTimeLeft = -1;
    bool shown = false;

    State state = STATE_SELECTING_GROUP;
    std::vector<std::string> groups{"video", "controls", "sound"};
    std::vector<std::vector<std::unique_ptr<TA_Option>>> options;
    TA_Sound switchSound, selectSound, backSound, errorSound;
    int group = 0, option = 0;

public:
    TA_OptionsMenu() {}
    void load(TA_Controller* controller);
    bool update();
    void draw();
    void reset();
    void show();
    void hide();
    bool isShown() {return shown || transitionTimeLeft > 0;}
};

#endif // TA_OPTIONS_MENU_H
