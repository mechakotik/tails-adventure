#include "data_select_section.h"
#include "save.h"
#include <bit>

void TA_DataSelectSection::load()
{
    entrySprite.load("data_select/entry.png");
    selectorRedSprite.load("data_select/selector.png", 48, 72);
    selectorWhiteSprite.load("data_select/selector.png", 48, 72);
    previewSprite.load("data_select/preview.png", 46, 46);
    optionsSprite.load("data_select/options.png");

    font.load("fonts/pause_menu.png", 8, 8);
    font.setMapping("abcdefghijklmnopqrstuvwxyz AB.?-0123456789CDEF%:");
    splashFont.load("fonts/splash.png", 7, 9);
    splashFont.setMapping(" !" + std::string{'"'} + "#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[a]^_`abcdefghijklmnopqrstuvwxyz{|}~");

    switchSound.load("sound/switch.ogg", TA_SOUND_CHANNEL_SFX1);
    selectSound.load("sound/select_item.ogg", TA_SOUND_CHANNEL_SFX1);
    loadSaveSound.load("sound/load_save.ogg", TA_SOUND_CHANNEL_SFX1);

    for(int pos = 0; pos < 9; pos ++) {
        TA_Point topLeft{menuStart + menuOffset * pos, (double)TA::screenHeight / 2 - entrySprite.getHeight() / 2};
        TA_Point bottomRight = topLeft + TA_Point(48, (pos == 0 ? 48 : 72));
        buttons[pos].setRectangle(topLeft, bottomRight);
    }

    splash = generateSplash();
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

    if(controller->isTouchscreen()) {
        updateScroll();
        if(updateTouchscreenSelection()) {
            return processSelection();
        }
    }
    else {
        updateSelection();
        if(controller->isJustPressed(TA_BUTTON_A) || controller->isJustPressed(TA_BUTTON_PAUSE)) {
            return processSelection();
        }
    }

    return TA_MAIN_MENU_DATA_SELECT;
}

TA_MainMenuState TA_DataSelectSection::processSelection()
{
    if(selection == 0) {
        selectSound.play();
        return TA_MAIN_MENU_OPTIONS;
    }

    TA::save::repairSave("save_" + std::to_string(selection - 1));
    TA::save::setCurrentSave("save_" + std::to_string(selection - 1));
    loadSaveSound.play();
    locked = true;
    return TA_MAIN_MENU_DATA_SELECT;
}

void TA_DataSelectSection::updateScroll()
{
    if(TA::touchscreen::isScrolling()) {
        scrollVelocity = TA::touchscreen::getScrollVector().x;
    }
    else if(!TA::equal(scrollVelocity, 0)) {
        if(scrollVelocity > 0) {
            scrollVelocity = std::max((double)0, scrollVelocity - scrollSlowdown * TA::elapsedTime);
        }
        else {
            scrollVelocity = std::min((double)0, scrollVelocity + scrollSlowdown * TA::elapsedTime);
        }
    }

    position += scrollVelocity;
    position = std::max(position, double(0));
    position = std::min(position, menuStart + 9 * menuOffset - TA::screenWidth);
}

void TA_DataSelectSection::updateSelection()
{
    // menuStart + selection * menuOffset + 24 - need = TA::screenWidth / 2
    double need = menuStart + selection * menuOffset + 24 - (double)TA::screenWidth / 2;
    need = std::max(need, double(0));
    need = std::min(need, menuStart + 9 * menuOffset - TA::screenWidth);

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
        if(selection + 1 < 9 && controller->getDirection() == TA_DIRECTION_RIGHT) {
            selection ++;
            switchSound.play();
        }
    }
}

bool TA_DataSelectSection::updateTouchscreenSelection()
{
    for(int pos = 0; pos < 9; pos ++) {
        buttons[pos].setPosition({-position, 0});
        buttons[pos].update();

        if(buttons[pos].isJustPressed() && !TA::touchscreen::isScrolling()) {
            scrollVelocity = 0;
        }
        else if(buttons[pos].isReleased()) {
            selection = pos;
            return true;
        }
    }

    return false;
}

void TA_DataSelectSection::draw()
{
    drawCustomEntries();
    drawSaveEntries();
    drawSplash();
    drawSelector();
}

void TA_DataSelectSection::drawCustomEntries()
{
    optionsSprite.setAlpha(alpha);
    optionsSprite.setPosition(menuStart - position, (double)TA::screenHeight / 2 - entrySprite.getHeight() / 2);
    optionsSprite.draw();
}

void TA_DataSelectSection::drawSaveEntries()
{
    entrySprite.setAlpha(alpha);
    previewSprite.setAlpha(alpha);
    font.setAlpha(255 * ((double)alpha / 255) * ((double)alpha / 255));

    for(int num = 0; num < 8; num ++) {
        TA_Point entryPosition{menuStart + (num + 1) * menuOffset - position, (double)TA::screenHeight / 2 - entrySprite.getHeight() / 2};
        entrySprite.setPosition(entryPosition);
        entrySprite.draw();

        if(TA::save::saveExists(num)) {
            previewSprite.setFrame(TA::save::getParameter("save_" + std::to_string(num) + "/last_unlocked"));
        }
        else {
            previewSprite.setFrame(0);
        }
        previewSprite.setPosition(entryPosition + TA_Point(1, 1));
        previewSprite.draw();

        font.drawText(entryPosition + TA_Point(11, 50), std::to_string(getSavePercent(num)) + "%");
        font.drawText(entryPosition + TA_Point(11, 60), getSaveTime(num), TA_Point(-2, 0));
    }
}

void TA_DataSelectSection::drawSplash()
{
    splashFont.setAlpha(alpha);
    splashFont.drawTextCentered(120, splash, TA_Point(-1, 0));
}

void TA_DataSelectSection::drawSelector()
{
    selectorTimer += TA::elapsedTime;
    selectorRedSprite.setAlpha(alpha);
    selectorWhiteSprite.setAlpha(TA::linearInterpolation(0, alpha, selectorTimer / selectorBlinkTime));

    int selectorPosition = -1;
    if(controller->isTouchscreen() && !locked) {
        selectorRedSprite.setAlpha(255);
        selectorWhiteSprite.setAlpha(0);
        for(int pos = 0; pos < 9; pos ++) {
            if(buttons[pos].isPressed()) {
                selectorPosition = pos;
            }
        }
    }
    else {
        selectorPosition = selection;
    }
    if(selectorPosition == -1) {
        return;
    }

    if(selectorPosition >= 1) {
        selectorRedSprite.setFrame(0);
        selectorWhiteSprite.setFrame(1);
    }
    else {
        selectorRedSprite.setFrame(2);
        selectorWhiteSprite.setFrame(3);
    }

    selectorRedSprite.setPosition(menuStart + selectorPosition * menuOffset - position, TA::screenHeight / 2 - selectorRedSprite.getHeight() / 2);
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

std::string TA_DataSelectSection::generateSplash()
{
    const std::vector<std::string> splashes {
        "test splash",
        "Gentoo maint btw",
        "Install Gentoo",
        "Also try Sonic Forces",
        "Forsy polutshe budut - arsedos",
        "Made with SDL3 o_o",
        "One Piece wa jitsuzai suru!"
    };

    return splashes[TA::random::next() % (int)splashes.size()];
}
