#include "sprite.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <numeric>
#include <toml.hpp>
#include <vector>
#include "error.h"
#include "filesystem.h"
#include "resource_manager.h"
#include "tools.h"

void TA_Texture::load(std::string filename) {
    SDLTexture = TA::resmgr::loadTexture(filename);
    float floatWidth, floatHeight;
    SDL_GetTextureSize(SDLTexture, &floatWidth, &floatHeight);
    width = int(floatWidth + 0.5);
    height = int(floatHeight + 0.5);
}

void TA_Animation::create(std::vector<int> newFrames, int newDelay, int newRepeatTimes) {
    frames = newFrames;
    delay = newDelay;
    repeatTimes = newRepeatTimes;
}

TA_Animation::TA_Animation(std::vector<int> newFrames, int newDelay, int newRepeatTimes) {
    create(newFrames, newDelay, newRepeatTimes);
}

TA_Animation::TA_Animation(int firstFrame, int lastFrame, int newDelay, int newRepeatTimes) {
    std::vector<int> newFrames(lastFrame - firstFrame + 1);
    std::iota(newFrames.begin(), newFrames.end(), firstFrame);
    create(newFrames, newDelay, newRepeatTimes);
}

TA_Animation::TA_Animation(int frame) {
    create(std::vector<int>{frame}, 1, -1);
}

void TA_Sprite::load(std::string filename, int newFrameWidth, int newFrameHeight) {
    texture.load(filename);
    if(newFrameWidth == -1) {
        frameWidth = texture.width;
        frameHeight = texture.height;
    } else {
        frameWidth = newFrameWidth;
        frameHeight = newFrameHeight;
    }

    animation = TA_Animation(0);
    loaded = true;
}

void TA_Sprite::loadFromToml(std::filesystem::path path) {
    try {
        tryLoadFromToml(path);
    } catch(std::exception& e) {
        TA::handleError("Failed to load %s:\n%s", path, e.what());
    }
}

void TA_Sprite::tryLoadFromToml(std::filesystem::path path) {
    const toml::value& table = TA::resmgr::loadToml(path);
    texture.load(path.parent_path() / table.at("sprite").at("image").as_string());
    if(table.at("sprite").contains("width")) {
        frameWidth = static_cast<int>(table.at("sprite").at("width").as_integer());
    } else {
        frameWidth = texture.width;
    }
    if(table.at("sprite").contains("height")) {
        frameHeight = static_cast<int>(table.at("sprite").at("height").as_integer());
    } else {
        frameHeight = texture.height;
    }

    animation = TA_Animation(0);
    loaded = true;
    if(!table.contains("animations")) {
        return;
    }

    for(const auto& [key, value] : table.at("animations").as_table()) {
        TA_Animation animation;
        if(value.is_integer()) {
            animation.frames = {static_cast<int>(value.as_integer())};
        } else if(value.is_table()) {
            const auto& frames = value.at("frames").as_array();
            animation.frames.resize(frames.size());
            for(int i = 0; i < frames.size(); i++) {
                animation.frames[i] = static_cast<int>(frames.at(i).as_integer());
            }
            if(value.contains("delay")) {
                animation.delay = static_cast<int>(value.at("delay").as_integer());
            }
            if(value.contains("repeat")) {
                animation.repeatTimes = static_cast<int>(value.at("repeat").as_integer());
            }
        }
        loadedAnimations[key] = animation;
    }
}

void TA_Sprite::draw() {
    drawFrom({-1, -1, -1, -1});
}

void TA_Sprite::drawFrom(SDL_Rect srcRect) {
    if(!loaded) {
        return;
    }
    updateAnimation();

    if(srcRect.x == -1) {
        srcRect.x = (frameWidth * frame) % texture.width;
    }
    if(srcRect.y == -1) {
        srcRect.y = (frameWidth * frame) / texture.width * frameHeight;
    }
    if(srcRect.w == -1) {
        srcRect.w = frameWidth;
    }
    if(srcRect.h == -1) {
        srcRect.h = frameHeight;
    }

    TA_Point cameraPosition;
    if(camera != nullptr) {
        cameraPosition = camera->getPosition();
    }

    SDL_Rect dstRect;
    dstRect.x = int(position.x * TA::scaleFactor + 0.5) - int(cameraPosition.x * TA::scaleFactor + 0.5);
    dstRect.y = int(position.y * TA::scaleFactor + 0.5) - int(cameraPosition.y * TA::scaleFactor + 0.5);
    dstRect.w = srcRect.w * TA::scaleFactor;
    dstRect.h = srcRect.h * TA::scaleFactor;

    if(!hidden) {
        SDL_SetTextureAlphaMod(texture.SDLTexture, alpha);
        SDL_FlipMode flipFlags = (flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
        SDL_FRect srcFRect, dstFRect;
        SDL_RectToFRect(&srcRect, &srcFRect);
        SDL_RectToFRect(&dstRect, &dstFRect);
        SDL_RenderTextureRotated(TA::renderer, texture.SDLTexture, &srcFRect, &dstFRect, 0, nullptr, flipFlags);
    }
    updateAnimationNeeded = true;
}

void TA_Sprite::updateAnimation() {
    if(!doUpdateAnimation || !updateAnimationNeeded) {
        return;
    }
    if(animation.frames.size() >= 2) {
        animationTimer += TA::elapsedTime;
        animationFrame += animationTimer / animation.delay;

        if(animationFrame >= (int)animation.frames.size()) {
            if(animation.repeatTimes != -1) {
                animation.repeatTimes -= animationFrame / animation.frames.size();
                if(animation.repeatTimes <= 0) {
                    setFrame(animation.frames.back());
                    frame = animation.frames[0];
                }
            }
            animationFrame %= animation.frames.size();
        }

        animationTimer = std::fmod(animationTimer, animation.delay);
        frame = animation.frames[animationFrame];
    } else {
        frame = animation.frames[0];
    }
    updateAnimationNeeded = false;
}

void TA_Sprite::setAnimation(TA_Animation newAnimation) {
    if(animation.frames == newAnimation.frames && animation.delay == newAnimation.delay) {
        return;
    }
    animation = newAnimation;
    animationFrame = animationTimer = 0;
}

void TA_Sprite::setAnimation(std::string name) {
    if(loadedAnimations.count(name)) {
        setAnimation(loadedAnimations[name]);
        animationName = name;
    } else {
        TA::printWarning("Unknown animation %s", name.c_str());
    }
}

void TA_Sprite::setFrame(int newFrame) {
    setAnimation(TA_Animation(newFrame));
}

bool TA_Sprite::isAnimated() {
    return animation.frames.size() >= 2;
}

void TA_Sprite::setAlpha(int newAlpha) {
    alpha = newAlpha;
    alpha = std::min(alpha, 255);
    alpha = std::max(alpha, 0);
    hidden = (alpha == 0);
}

void TA_Sprite::setColorMod(int r, int g, int b) {
    auto normalize = [&](int x) {
        x = std::min(x, 255);
        x = std::max(x, 0);
        return x;
    };
    r = normalize(r);
    g = normalize(g);
    b = normalize(b);
    SDL_SetTextureColorMod(texture.SDLTexture, r, g, b);
}

int TA_Sprite::getAnimationFrame() {
    updateAnimation();
    return animationFrame;
}

int TA_Sprite::getCurrentFrame() {
    updateAnimation();
    return frame;
}
