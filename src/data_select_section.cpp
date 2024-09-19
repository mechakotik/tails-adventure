#include "data_select_section.h"
#include "save.h"
#include <bit>

void TA_DataSelectSection::load()
{
    entrySprite.load("data_select/entry.png");
    selectorRedSprite.load("data_select/selector.png", 48, 72);
    selectorRedSprite.setFrame(0);
    selectorWhiteSprite.load("data_select/selector.png", 48, 72);
    selectorWhiteSprite.setFrame(1);
    previewSprite.load("data_select/preview.png", 46, 45);

    font.load("fonts/pause_menu.png", 8, 8);
    font.setMapping("abcdefghijklmnopqrstuvwxyz AB.?-0123456789CDEF%:");

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
    font.setAlpha(alpha);

    for(int num = 0; num < 8; num ++) {
        TA_Point entryPosition{menuStart + num * menuOffset - position, (double)TA::screenHeight / 2 - entrySprite.getHeight() / 2};
        entrySprite.setPosition(entryPosition);
        entrySprite.draw();

        if(TA::save::saveExists(num)) {
            previewSprite.setPosition(entryPosition + TA_Point(1, 1));
            previewSprite.setFrame(TA::save::getParameter("save_" + std::to_string(num) + "/last_unlocked") - 1);
            previewSprite.draw();
        }

        font.drawText(entryPosition + TA_Point(11, 49), std::to_string(getSavePercent(num)) + "%");
        font.drawText(entryPosition + TA_Point(11, 59), getSaveTime(num), TA_Point(-2, 0));
    }

    selectorRedSprite.setPosition(menuStart + selection * menuOffset - position, TA::screenHeight / 2 - selectorRedSprite.getHeight() / 2);
    selectorWhiteSprite.setPosition(selectorRedSprite.getPosition());
    selectorRedSprite.draw();
    selectorWhiteSprite.draw();
}

int TA_DataSelectSection::getSavePercent(int save)
{
    if(!TA::save::saveExists(save)) {
        return 0;
    }

    std::string saveName = "save_" + std::to_string(save);
    int areaCount = std::popcount((unsigned long long)TA::save::getParameter(saveName + "/area_mask"));
    int itemCount = std::popcount((unsigned long long)TA::save::getParameter(saveName + "/item_mask"));
    return 50 * (double(areaCount - 4) / 11 + double(itemCount - 2) / 30);
}

std::string TA_DataSelectSection::getSaveTime(int save)
{
    if(!TA::save::saveExists(save)) {
        return "--:--";
    }

    std::string saveName = "save_" + std::to_string(save);
    int minutes = TA::save::getParameter(saveName + "/time") / 3600;
    int hours = minutes / 60;
    minutes %= 60;
    hours %= 100;

    if(minutes <= 9) {
        return std::to_string(hours) + ":0" + std::to_string(minutes);
    }
    return std::to_string(hours) + ":" + std::to_string(minutes);
}
