#include "item_ratio_screen.h"
#include "save.h"
#include "tools.h"

namespace {
    int getItemRatio() {
        int itemCount = std::popcount(static_cast<uint64_t>(TA::save::getSaveParameter("item_mask")));
        return 100 * (itemCount - 2) / 30;
    }
} // namespace

void TA_ItemRatioScreen::init() {
    font.loadFont("fonts/pause_menu.toml");
    ratio = getItemRatio();
}

TA_ScreenState TA_ItemRatioScreen::update() {
    timer += TA::elapsedTime;

    float yOffset = (TA::screenHeight / 2) - 72;
    font.drawTextCentered(yOffset + 48, "item detection");
    font.drawTextCentered(yOffset + 64, "ratio");
    font.drawTextCentered(yOffset + 88, std::to_string(ratio) + " percent");

    if(timer > 420) {
        return TA_SCREENSTATE_TITLE;
    }
    return TA_SCREENSTATE_CURRENT;
}
