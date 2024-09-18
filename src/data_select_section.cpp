#include "data_select_section.h"
#include "save.h"

void TA_DataSelectSection::load()
{
    entrySprite.load("data_select/entry.png");
    selectorRedSprite.load("data_select/selector.png", 48, 72);
    selectorRedSprite.setFrame(0);
    selectorWhiteSprite.load("data_select/selector.png", 48, 72);
    selectorWhiteSprite.setFrame(1);

    switchSound.load("sound/switch.ogg", TA_SOUND_CHANNEL_SFX1);
    loadSaveSound.load("sound/load_save.ogg", TA_SOUND_CHANNEL_SFX1);
}

TA_MainMenuState TA_DataSelectSection::update()
{
    if(locked) {
        timer += TA::elapsedTime;
        if(timer > loadTime) {
            return TA_MAIN_MENU_EXIT;
        }
        return TA_MAIN_MENU_DATA_SELECT;
    }

    updateSelection();

    if(controller->isJustPressed(TA_BUTTON_A) || controller->isJustPressed(TA_BUTTON_PAUSE)) {
        TA::save::repairSave("save_" + std::to_string(selection));
        TA::save::setCurrentSave("save_" + std::to_string(selection));
        loadSaveSound.play();
        locked = true;
    }

    return TA_MAIN_MENU_DATA_SELECT;
}

void TA_DataSelectSection::updateSelection()
{
    // menuStart + selection * menuOffset + 24 - need = TA::screenWidth / 2
    double need = menuStart + selection * menuOffset + 24 - (double)TA::screenWidth / 2;
    need = std::max(need, double(0));
    need = std::min(need, menuStart + 8 * menuOffset - TA::screenWidth);

    if(!TA::equal(position, need)) {
        if(position > need) {
            position = std::max(need, position - scrollSpeed * TA::elapsedTime);
        }
        if(position < need) {
            position = std::min(need, position + scrollSpeed * TA::elapsedTime);
        }
    }
    else if(controller->isJustChangedDirection()) {
        if(selection - 1 >= 0 && controller->getDirection() == TA_DIRECTION_LEFT) {
            selection --;
            switchSound.play();
        }
        if(selection + 1 < 8 && controller->getDirection() == TA_DIRECTION_RIGHT) {
            selection ++;
            switchSound.play();
        }
    }
}

void TA_DataSelectSection::draw()
{
    selectorTimer += TA::elapsedTime;

    selectorRedSprite.setAlpha(alpha);
    selectorWhiteSprite.setAlpha(TA::linearInterpolation(0, alpha, selectorTimer / selectorBlinkTime));
    entrySprite.setAlpha(alpha);

    for(int num = 0; num < 8; num ++) {
        entrySprite.setPosition(menuStart + num * menuOffset - position, TA::screenHeight / 2 - entrySprite.getHeight() / 2);
        entrySprite.draw();
    }

    selectorRedSprite.setPosition(menuStart + selection * menuOffset - position, TA::screenHeight / 2 - selectorRedSprite.getHeight() / 2);
    selectorWhiteSprite.setPosition(selectorRedSprite.getPosition());
    selectorRedSprite.draw();
    selectorWhiteSprite.draw();
}
