#include "sparkle.h"

void TA_Sparkle::load(TA_Point position) {
    loadFromToml("objects/sparkle.toml");
    setAnimation("sparkle");
    this->position = position;
    updatePosition();
}

bool TA_Sparkle::update() {
    return isAnimated();
}
