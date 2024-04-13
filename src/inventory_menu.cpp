#include "inventory_menu.h"
#include "save.h"

void TA_InventoryMenu::load(TA_Controller* controller)
{
    this->controller = controller;

    itemSprite.load("hud/items.png", 16, 16);
    inventoryItemSprite.load("hud/items.png", 16, 16);
    pointerSprite.load("house/pointer.png");
    inventoryPointerSprite.load("house/pointer.png");
    arrowSprite.load("house/arrow.png", 8, 13);

    font.load("fonts/item.png", 8, 8);
    font.setMapping("abcdefghijklmnopqrstuvwxyz AB.?");

    switchSound.load("sound/switch.ogg", TA_SOUND_CHANNEL_SFX1);
    selectSound.load("sound/select_item.ogg", TA_SOUND_CHANNEL_SFX2);
    backSound.load("sound/select.ogg", TA_SOUND_CHANNEL_SFX2);
    errorSound.load("sound/damage.ogg", TA_SOUND_CHANNEL_SFX2);

    fillItemMatrix();
}

void TA_InventoryMenu::fillItemMatrix()
{
    items[0][0] = {0, "regular bomb"};
    items[0][1] = {14, "triple bomb"};
    items[1][0] = {1, "large bomb"};
    items[1][1] = {15, "wrench"};
    items[2][0] = {2, "remote bomb"};
    items[2][1] = {16, "helmet"};
    items[3][0] = {13, "napalm bomb"};
    items[3][1] = {6, "remote robot"};
    items[4][0] = {19, "hammer"};
    items[4][1] = {8, "super glove"};
    items[5][0] = {3, "teleport device"};
    items[5][1] = {9, "fang"};
    items[6][0] = {5, "night vision"};
    items[6][1] = {10, "knuckles"};
    items[7][0] = {7, "speed boots"};
    items[7][1] = {11, "sonic"};
    items[8][0] = {17, "item radar"};
    items[8][1] = {33, "purple c.emerald"};
    items[9][0] = {18, "radio"};
    items[9][1] = {31, "red c.emerald"};
    items[10][0] = {32, "blue c.emerald"};
    items[10][1] = {30, "white c.emerald"};
    items[11][0] = {29, "green c.emerald"};
    items[11][1] = {34, "yellow c.emerald"};

    for(int x = 0; x < 12; x ++) {
        for(int y = 0; y < 2; y ++) {
            itemName[items[x][y].number] = items[x][y].name;
        }
    }
}

bool TA_InventoryMenu::update()
{
    if(showTimeLeft <= 0 && hideTimeLeft <= 0 && listTransitionTimeLeft <= 0) {
        if(selectingSlot) {
            return updateSlotSelection();
        }
        updateItemSelection();
    }

    return true;
}

bool TA_InventoryMenu::updateSlotSelection()
{
    if(controller->isJustChangedDirection()) {
        TA_Direction direction = controller->getDirection();
        int prev = selectionSlot;

        if(direction == TA_DIRECTION_LEFT) {
            selectionSlot = std::max(0, selectionSlot - 1);
        }
        else if(direction == TA_DIRECTION_RIGHT) {
            selectionSlot = std::min(3, selectionSlot + 1);
        }

        if(selectionSlot != prev) {
            switchSound.play();
        }
    }

    if(controller->isJustPressed(TA_BUTTON_A)) {
        selectSound.play();
        selectingSlot = false;
    }
    if(controller->isJustPressed(TA_BUTTON_B)) {
        backSound.play();
        return false;
    }
    return true;
}

void TA_InventoryMenu::updateItemSelection()
{
    if(controller->isJustChangedDirection()) {
        TA_Direction direction = controller->getDirection();
        prevSelection = {selectionX, selectionY};

        switch(direction) {
            case TA_DIRECTION_UP:
                selectionY = std::max(0, selectionY - 1);
                break;
            case TA_DIRECTION_DOWN:
                selectionY = std::min(1, selectionY + 1);
                break;
            case TA_DIRECTION_LEFT:
                selectionX = std::max(0, selectionX - 1);
                break;
            case TA_DIRECTION_RIGHT:
                selectionX = std::min(11, selectionX + 1);
                break;
            default:
                break;
        }

        if(prevSelection != std::make_pair(selectionX, selectionY)) {
            switchSound.play();
            if(prevSelection.first / 4 != selectionX / 4) {
                listTransitionTimeLeft = listTransitionTime * 2;
            }
        }
    }

    if(controller->isJustPressed(TA_BUTTON_A)) {
        if(canPlaceItem(items[selectionX][selectionY].number)) {
            placeItem(selectionSlot, items[selectionX][selectionY].number);
            selectSound.play();
        }
        else {
            errorSound.play();
        }
        selectingSlot = true;
    }
    if(controller->isJustPressed(TA_BUTTON_B)) {
        selectingSlot = true;
        backSound.play();
    }
}

bool TA_InventoryMenu::canPlaceItem(int item)
{
    if(!characterHasItem(item)) {
        return false;
    }
    if(item >= 29) {
        return false;
    }
    return true;
}

void TA_InventoryMenu::placeItem(int slot, int item)
{
    int prevSlot = -1;
    for(int pos = 0; pos < 4; pos ++) {
        if(getInventoryItem(pos) == item) {
            prevSlot = pos;
            break;
        }
    }

    if(prevSlot == -1) {
        setInventoryItem(slot, item);
    }
    else {
        int prevItem = getInventoryItem(slot);
        setInventoryItem(slot, item);
        setInventoryItem(prevSlot, prevItem);
    }
}

void TA_InventoryMenu::draw()
{
    updateAlpha();
    drawItemList();
    drawInventory();
    drawSelectionName();
    drawPointer();
    drawArrows();
}

void TA_InventoryMenu::updateAlpha()
{
    if(showTimeLeft > 0) {
        showTimeLeft -= TA::elapsedTime;
    }
    if(hideTimeLeft > 0) {
        hideTimeLeft -= TA::elapsedTime;
    }

    int globalAlpha = 0;
    if(showTimeLeft > 0) {
        globalAlpha = 255 - 255 * showTimeLeft / transitionTime;
    }
    else if(hideTimeLeft > 0) {
        globalAlpha = 255 * hideTimeLeft / transitionTime;
    }
    else if(shown) {
        globalAlpha = 255;
    }

    inventoryItemSprite.setAlpha(globalAlpha);
    font.setAlpha(globalAlpha);
    inventoryPointerSprite.setAlpha(globalAlpha);

    arrowTimer += TA::elapsedTime;
    arrowTimer = fmod(arrowTimer, (arrowIdleTime + arrowTransitionTime) * 2);
    int arrowAlpha = 0;

    if(arrowTimer < arrowTransitionTime) {
        arrowAlpha = 255 * arrowTimer / arrowTransitionTime;
    }
    else if(arrowTimer < arrowTransitionTime + arrowIdleTime) {
        arrowAlpha = 255;
    }
    else if(arrowTimer < arrowTransitionTime * 2 + arrowIdleTime) {
        arrowAlpha = 255 - 255 * (arrowTimer - arrowIdleTime - arrowTransitionTime) / arrowTransitionTime;
    }
    else {
        arrowAlpha = 0;
    }

    arrowAlpha = std::min(arrowAlpha, globalAlpha);
    arrowSprite.setAlpha(arrowAlpha);

    int itemAlpha = 255;

    if(listTransitionTimeLeft > 0) {
        listTransitionTimeLeft -= TA::elapsedTime;
        if(listTransitionTimeLeft > listTransitionTime) {
            itemAlpha = 255 * (listTransitionTimeLeft - listTransitionTime) / listTransitionTime;
        }
        else {
            itemAlpha = 255 - 255 * listTransitionTimeLeft / listTransitionTime;
        }
    }

    itemAlpha = std::min(itemAlpha, globalAlpha);
    itemSprite.setAlpha(itemAlpha);
    pointerSprite.setAlpha(itemAlpha);
}

void TA_InventoryMenu::drawItemList()
{
    int page = selectionX / 4;
    if(listTransitionTimeLeft > listTransitionTime) {
        page = prevSelection.first / 4;
    }
    
    for(int x = page * 4; x < (page + 1) * 4; x ++) {
        for(int y = 0; y < 2; y ++) {
            double screenX = getLeftX() + 24 + 32 * (x % 4);
            itemSprite.setPosition(screenX, (y == 0? 40 : 64));

            int index = items[x][y].number;
            if(characterHasItem(index)) {
                itemSprite.setFrame(index);
            }
            else {
                itemSprite.setFrame(38);
            }

            itemSprite.draw();
        }
    }
}

void TA_InventoryMenu::drawInventory()
{
    inventoryItemSprite.setPosition(getLeftX() + 39, 101);

    for(int pos = 0; pos < 4; pos ++) {
        int index = getInventoryItem(pos);
        if(index == -1) {
            index = 38;
        }
        inventoryItemSprite.setFrame(index);
        inventoryItemSprite.draw();
        inventoryItemSprite.setPosition(inventoryItemSprite.getPosition() + TA_Point(22, 0));
    }
}

int TA_InventoryMenu::getInventoryItem(int slot)
{
    std::string paramName = "item_slot" + std::to_string(slot);
    return TA::save::getSaveParameter(paramName);
}

void TA_InventoryMenu::setInventoryItem(int slot, int index)
{
    std::string paramName = "item_slot" + std::to_string(slot);
    TA::save::setSaveParameter(paramName, index);
}

void TA_InventoryMenu::drawSelectionName()
{
    TA_Point position = TA_Point(getLeftX() + 16, 128);
    std::string text = "";

    if(selectingSlot) {
        int item = getInventoryItem(selectionSlot);
        if(itemName.count(item)) {
            text = itemName[item];
        }
    }
    else {
        if(characterHasItem(items[selectionX][selectionY].number)) {
            text = items[selectionX][selectionY].name;
        }
        else {
            text = "???";
        }
    }

    font.drawText(position, text);
}

bool TA_InventoryMenu::characterHasItem(int item)
{
    long long itemMask = TA::save::getSaveParameter("item_mask");
    return itemMask & (1ll << item);
}

void TA_InventoryMenu::drawPointer()
{
    TA_Point inventoryPosition = TA_Point(getLeftX() + 39 + 22 * selectionSlot, 101);
    inventoryPointerSprite.setPosition(inventoryPosition - TA_Point(1, 2));
    inventoryPointerSprite.draw();

    if(!selectingSlot) {
        int pointerX = selectionX, pointerY = selectionY;
        if(listTransitionTimeLeft > listTransitionTime) {
            std::tie(pointerX, pointerY) = prevSelection;
        }

        TA_Point listPosition = TA_Point(getLeftX() + 24 + 32 * (pointerX % 4), (pointerY == 0? 40 : 64));
        pointerSprite.setPosition(listPosition - TA_Point(1, 2));
        pointerSprite.draw();
    }
}

void TA_InventoryMenu::drawArrows()
{
    if(selectionX >= 4) {
        arrowSprite.setPosition(getLeftX() + 8, 57);
        arrowSprite.setFrame(0);
        arrowSprite.draw();
    }
    if(selectionX <= 7) {
        arrowSprite.setPosition(getLeftX() + 144, 57);
        arrowSprite.setFrame(1);
        arrowSprite.draw();
    }
}

void TA_InventoryMenu::show()
{
    if(shown) {
        return;
    }
    showTimeLeft = transitionTime;
    hideTimeLeft = -1;
    shown = true;
    selectionSlot = selectionX = selectionY = 0;
}

void TA_InventoryMenu::hide()
{
    if(!shown) {
        return;
    }
    hideTimeLeft = transitionTime;
    showTimeLeft = -1;
    shown = false;
}
