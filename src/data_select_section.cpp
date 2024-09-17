#include "data_select_section.h"
#include "error.h"

void TA_DataSelectSection::load()
{
    entrySprite.load("data_select/entry.png");
}

TA_MainMenuState TA_DataSelectSection::update()
{
    return TA_MAIN_MENU_DATA_SELECT;
}

void TA_DataSelectSection::setAlpha(int alpha)
{

}

void TA_DataSelectSection::draw()
{
    for(int num = 0; num < 8; num ++) {
        entrySprite.setPosition(menuStart + num * menuOffset - position, TA::screenHeight / 2 - entrySprite.getHeight() / 2);
        entrySprite.draw();
    }
}
