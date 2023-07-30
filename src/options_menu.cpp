#include "options_menu.h"
#include "save.h"

class TA_ResolutionOption : public TA_Option {
public:
    std::string getName() override {return "resolution";}

    std::string getValue() override {
        int factor = TA::save::getParameter("resolution");
        return "x" + std::to_string(factor);
    }

    TA_MoveSoundId move(int delta) override {
        int factor = TA::save::getParameter("resolution");
        factor += delta;

        TA_MoveSoundId result = TA_MOVE_SOUND_SWITCH;
        if(factor < 1) {
            factor = 1;
            result = TA_MOVE_SOUND_ERROR;
        }
        if(factor > 20) {
            factor = 20;
            result = TA_MOVE_SOUND_ERROR;
        }

        TA::save::setParameter("resolution", factor);
        return result;
    }
};

class TA_VSyncOption : public TA_Option {
public:
    std::string getName() override {return "v-sync";}

    std::string getValue() override {
        int value = TA::save::getParameter("vsync");
        return (value == 1 ? "on" : "off");
    }

    TA_MoveSoundId move(int delta) override {
        int value = TA::save::getParameter("vsync");
        value = 1 - value;
        TA::save::setParameter("vsync", value);
        return TA_MOVE_SOUND_SWITCH;
    }
};

class TA_MaxFPSOption : public TA_Option {
public:
    std::string getName() override {return "fps limit";}

    std::string getValue() override {
        int value = TA::save::getParameter("fps_limit");
        if(value == 0) {
            return "off";
        }
        return std::to_string(TA::fpsLimits[value - 1]);
    }

    TA_MoveSoundId move(int delta) override {
        int value = TA::save::getParameter("fps_limit");
        value += delta;

        TA_MoveSoundId result = TA_MOVE_SOUND_SWITCH;
        if(value < 0) {
            value = 0;
            result = TA_MOVE_SOUND_ERROR;
        }
        if(value > (int)TA::fpsLimits.size()) {
            value = (int)TA::fpsLimits.size();
            result = TA_MOVE_SOUND_ERROR;
        }

        TA::save::setParameter("fps_limit", value);
        return result;
    }
};

class TA_VolumeOption : public TA_Option {
    std::string name, param;

public:
    TA_VolumeOption(std::string name, std::string param) {
        this->name = name;
        this->param = param;
    }

    std::string getName() override {return name;}

    std::string getValue() override {
        int value = TA::save::getParameter(param);
        std::string bar(10, 'B');
        bar[0] = 'S';
        bar[9] = 'F';
        for(int i = 1; i <= value; i ++) {
            bar[i] = 'A';
        }
        return bar;
    }

    TA_MoveSoundId move(int delta) override {
        int value = TA::save::getParameter(param);
        value += delta;

        TA_MoveSoundId result = TA_MOVE_SOUND_SWITCH;
        if(value < 0) {
            value = 0;
            result = TA_MOVE_SOUND_ERROR;
        }
        if(value > 8) {
            value = 8;
            result = TA_MOVE_SOUND_ERROR;
        }

        TA::save::setParameter(param, value);
        return result;
    }
};

void TA_OptionsMenu::load(TA_CommonController* controller)
{
    this->controller = controller;
    activeFont.load("fonts/item.png", 8, 8);
    activeFont.setMapping("abcdefghijklmnopqrstuvwxyz XY.?-0123456789SABF");
    inactiveFont.load("fonts/item_inactive.png", 8, 8);
    inactiveFont.setMapping("abcdefghijklmnopqrstuvwxyz XY.?-0123456789SABF");

    options.resize(groups.size());
    options[0].push_back(new TA_ResolutionOption());
    options[0].push_back(new TA_VSyncOption());
    options[0].push_back(new TA_MaxFPSOption());

    options[2].push_back(new TA_VolumeOption("main", "main_volume"));
    options[2].push_back(new TA_VolumeOption("music", "music_volume"));
    options[2].push_back(new TA_VolumeOption("sfx", "sfx_volume"));

    switchSound.load("sound/switch.ogg", TA_SOUND_CHANNEL_SFX1);
    selectSound.load("sound/select_item.ogg", TA_SOUND_CHANNEL_SFX2);
    backSound.load("sound/select.ogg", TA_SOUND_CHANNEL_SFX2);
    errorSound.load("sound/damage.ogg", TA_SOUND_CHANNEL_SFX3);
}

bool TA_OptionsMenu::update()
{
    if(menuTransitionTimeLeft > 0) {
        return true;
    }

    switch(state) {
        case STATE_SELECTING_GROUP:
            updateGroupSelector();
            break;
        case STATE_SELECTING_OPTION:
            updateOptionSelector();
            break;
        case STATE_QUIT:
            return false;
    }

    return true;
}

void TA_OptionsMenu::updateGroupSelector()
{
    if(controller->isJustChangedDirection()) {
        TA_Direction direction = controller->getDirection();
        if(direction == TA_DIRECTION_UP && group > 0) {
            group --;
            switchSound.play();
        }
        else if(direction == TA_DIRECTION_DOWN && group < (int)groups.size() - 1) {
            group ++;
            switchSound.play();
        }
    }

    if(controller->isJustPressed(TA_BUTTON_A)) {
        menuTransitionTimeLeft = transitionTime * 2;
        option = 0;
        selectSound.play();
        return;
    }

    if(controller->isJustPressed(TA_BUTTON_B)) {
        state = STATE_QUIT;
        backSound.play();
    }
}

void TA_OptionsMenu::updateOptionSelector()
{
    TA_MoveSoundId sound = TA_MOVE_SOUND_EMPTY;

    if(controller->isJustChangedDirection()) {
        TA_Direction direction = controller->getDirection();

        switch(direction) {
            case TA_DIRECTION_UP:
                if(option > 0) {
                    option --;
                    sound = TA_MOVE_SOUND_SWITCH;
                }
                break;
            case TA_DIRECTION_DOWN:
                if(option < (int)options[group].size() - 1) {
                    option ++;
                    sound = TA_MOVE_SOUND_SWITCH;
                }
                break;
            case TA_DIRECTION_LEFT:
                sound = options[group][option]->move(-1);
                break;
            case TA_DIRECTION_RIGHT:
                sound = options[group][option]->move(1);
                break;
            default:
                break;
        }
    }

    if(controller->isJustPressed(TA_BUTTON_A)) {
        sound = options[group][option]->move(1);
    }

    switch(sound) {
        case TA_MOVE_SOUND_SWITCH:
            switchSound.play();
            break;
        case TA_MOVE_SOUND_SELECT:
            selectSound.play();
            break;
        case TA_MOVE_SOUND_ERROR:
            errorSound.play();
            break;
        default:
            break;
    }

    if(controller->isJustPressed(TA_BUTTON_B)) {
        menuTransitionTimeLeft = transitionTime * 2;
        backSound.play();
    }
}

void TA_OptionsMenu::draw()
{
    updateAlpha();

    if(state == STATE_SELECTING_OPTION) {
        drawOptionList();
    }
    else {
        drawGroupList();
    }
}

void TA_OptionsMenu::drawGroupList()
{
    TA_Point textPosition{getLeftX() + 16, 36};
    for(int pos = 0; pos < (int)groups.size(); pos ++) {
        if(pos == group) {
            activeFont.drawText(textPosition, groups[pos]);
        }
        else {
            inactiveFont.drawText(textPosition, groups[pos]);
        }
        textPosition.y += 10;
    }
}

void TA_OptionsMenu::drawOptionList()
{
    double lx = getLeftX() + 16, rx = getLeftX() + 144, y = 36;
    for(int pos = 0; pos < (int)options[group].size(); pos ++) {
        std::string left = options[group][pos]->getName();
        std::string right = options[group][pos]->getValue();
        int offset = activeFont.getTextWidth(right, {0, 0});

        if(pos == option) {
            activeFont.drawText({lx, y}, left);
            activeFont.drawText({rx - offset, y}, right);
        }
        else {
            inactiveFont.drawText({lx, y}, left);
            inactiveFont.drawText({rx - offset, y}, right);
        }
        y += 10;
    }
}

void TA_OptionsMenu::updateAlpha()
{
    double alpha = 0;

    if(transitionTimeLeft > 0) {
        transitionTimeLeft -= TA::elapsedTime;
        if(shown) {
            alpha = 255 - 255 * transitionTimeLeft / transitionTime;
        }
        else {
            alpha = 255 * transitionTimeLeft / transitionTime;
        }
    }
    else if(menuTransitionTimeLeft > 0) {
        bool flag1 = (menuTransitionTimeLeft > transitionTime);
        menuTransitionTimeLeft -= TA::elapsedTime;
        bool flag2 = (menuTransitionTimeLeft > transitionTime);

        if(flag1 != flag2) {
            state = (state == STATE_SELECTING_GROUP ? STATE_SELECTING_OPTION : STATE_SELECTING_GROUP);
        }
        if(menuTransitionTimeLeft > transitionTime) {
            alpha = 255 * (menuTransitionTimeLeft - transitionTime) / transitionTime;
        }
        else {
            alpha = 255 - 255 * menuTransitionTimeLeft / transitionTime;
        }
    }
    else if(shown) {
        alpha = 255;
    }
    else {
        alpha = 0;
    }

    activeFont.setAlpha(alpha);
    inactiveFont.setAlpha(alpha);
}

void TA_OptionsMenu::reset()
{
    state = STATE_SELECTING_GROUP;
    group = 0;
}

void TA_OptionsMenu::show()
{
    if(shown) {
        return;
    }
    shown = true;
    transitionTimeLeft = transitionTime;
}

void TA_OptionsMenu::hide()
{
    if(!shown) {
        return;
    }
    shown = false;
    transitionTimeLeft = transitionTime;
}
