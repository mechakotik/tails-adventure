#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <sstream>
#include "tinyxml2.h"
#include "sprite.h"
#include "error.h"
#include "tools.h"

void TA_Texture::load(std::string filename)
{
    SDL_Surface *surface = IMG_Load(filename.c_str());
    if(surface == nullptr) {
        TA::handleSDLError("Failed to load image");
    }

    SDLTexture = SDL_CreateTextureFromSurface(TA::renderer, surface);
    if(SDLTexture == nullptr) {
        TA::handleSDLError("Failed to create texture from surface");
    }
    SDL_SetTextureBlendMode(SDLTexture, SDL_BLENDMODE_BLEND);

    width = surface->w;
    height = surface->h;
    SDL_FreeSurface(surface);
}

TA_Texture::~TA_Texture()
{
    SDL_DestroyTexture(SDLTexture);
}

void TA_Animation::create(std::vector<int> newFrames, int newDelay, int newRepeatTimes)
{
    frames = newFrames;
    delay = newDelay;
    repeatTimes = newRepeatTimes;
}

TA_Animation::TA_Animation(std::vector<int> newFrames, int newDelay, int newRepeatTimes)
{
    create(newFrames, newDelay, newRepeatTimes);
}

TA_Animation::TA_Animation(int firstFrame, int lastFrame, int newDelay, int newRepeatTimes)
{
    std::vector<int> newFrames(lastFrame - firstFrame + 1);
    std::iota(newFrames.begin(), newFrames.end(), firstFrame);
    create(newFrames, newDelay, newRepeatTimes);
}

TA_Animation::TA_Animation(int frame)
{
    create(std::vector<int>{frame}, 1, -1);
}

void TA_Sprite::load(std::string filename, int newFrameWidth, int newFrameHeight)
{
    TA_Texture *newTexture = new TA_Texture();
    newTexture->load(filename);
    loadFromTexture(newTexture, newFrameWidth, newFrameHeight);
}

void TA_Sprite::loadFromTexture(TA_Texture *newTexture, int newFrameWidth, int newFrameHeight)
{
    texture = newTexture;
    texture->spritesUsed ++;

    if(newFrameWidth == -1) {
        frameWidth = texture->width;
        frameHeight = texture->height;
    }
    else {
        frameWidth = newFrameWidth;
        frameHeight = newFrameHeight;
    }

    animation = TA_Animation(0);
}

void TA_Sprite::draw()
{
    updateAnimation();
    SDL_Rect srcRect, dstRect;
    srcRect.x = (frameWidth * frame) % texture->width;
    srcRect.y = (frameWidth * frame) / texture->width * frameHeight;
    srcRect.w = frameWidth;
    srcRect.h = frameHeight;

    TA_Point screenPosition = position;
    if(camera != nullptr) {
        screenPosition = camera->getRelative(position);
    }

    dstRect.x = screenPosition.x * TA::widthMultiplier;
    dstRect.y = screenPosition.y * TA::heightMultiplier;
    dstRect.w = frameWidth * (noPixelAspectRatio ? TA::heightMultiplier : TA::widthMultiplier) * scale.x + 1;
    dstRect.h = frameHeight * TA::heightMultiplier * scale.y + 1;
    if(!hidden) {
        SDL_RendererFlip flipFlags = (flip? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
        SDL_RenderCopyEx(TA::renderer, texture->SDLTexture, &srcRect, &dstRect, 0, nullptr, flipFlags);
    }
    updateAnimationNeeded = true;
}

void TA_Sprite::updateAnimation()
{
    if(!updateAnimationNeeded) {
        return;
    }
    if(animation.frames.size() >= 2) {
        animationTimer += TA::elapsedTime;
        animationFrame += animationTimer / animation.delay;

        if (animationFrame >= animation.frames.size()) {
            if (animation.repeatTimes != -1) {
                animation.repeatTimes -= animationFrame / animation.frames.size();
                if (animation.repeatTimes <= 0) {
                    setFrame(animation.frames.back());
                    frame = animation.frames[0];
                }
            }
            animationFrame %= animation.frames.size();
        }

        animationTimer = std::fmod(animationTimer, animation.delay);
        frame = animation.frames[animationFrame];
    }
    else{
        frame = animation.frames[0];
    }
    updateAnimationNeeded = false;
}

void TA_Sprite::loadAnimationsFromFile(std::string filename)
{
    tinyxml2::XMLDocument animationXml;
    animationXml.Parse(TA::readStringFromFile(filename).c_str());
    tinyxml2::XMLElement *currentElement = animationXml.RootElement()->FirstChildElement("animation");

    while(currentElement != nullptr) {
        std::string name = currentElement->Attribute("name");
        std::vector<int> frames; {
            std::stringstream framesStr;
            framesStr << currentElement->GetText();
            int currentFrame;
            char temp;
            while(framesStr >> currentFrame) {
                frames.push_back(currentFrame);
                framesStr >> temp;
            }
        }
        int delay = currentElement->IntAttribute("delay", 1);
        int repeatTimes = currentElement->IntAttribute("repeatTimes", -1);
        loadedAnimations[name] = TA_Animation(frames, delay, repeatTimes);
        currentElement = currentElement->NextSiblingElement("animation");
    }
}

void TA_Sprite::setAnimation(TA_Animation newAnimation)
{
    if(animation.frames == newAnimation.frames) {
        return;
    }
    animation = newAnimation;
    animationFrame = animationTimer = 0;
}

void TA_Sprite::setAnimation(std::string name)
{
    setAnimation(loadedAnimations[name]);
}

void TA_Sprite::setFrame(int newFrame)
{
    setAnimation(TA_Animation(newFrame));
}

bool TA_Sprite::isAnimated()
{
    return animation.frames.size() >= 2;
}

void TA_Sprite::setAlpha(int alpha)
{
    alpha = std::min(alpha, 255);
    alpha = std::max(alpha, 0);
    hidden = (alpha == 0);
    SDL_SetTextureAlphaMod(texture->SDLTexture, alpha);
}

void TA_Sprite::setColorMod(int r, int g, int b)
{
    auto normalize = [&](int x) {
        x = std::min(x, 255);
        x = std::max(x, 0);
        return x;
    };
    r = normalize(r);
    g = normalize(g);
    b = normalize(b);
    SDL_SetTextureColorMod(texture->SDLTexture, r, g, b);
}

int TA_Sprite::getAnimationFrame()
{
    updateAnimation();
    return animationFrame;
}

TA_Sprite::~TA_Sprite()
{
    texture->spritesUsed --;
    if(texture->spritesUsed == 0) {
        delete texture;
    }
}
