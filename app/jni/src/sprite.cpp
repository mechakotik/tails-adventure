#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>
#include "sprite.h"
#include "globals.h"
#include "error.h"

void TA_Texture::load(std::string filename)
{
    SDL_Surface *surface = IMG_Load(filename.c_str());
    if(surface == nullptr) {
        handleSDLError("Failed to load image");
    }

    SDLTexture = SDL_CreateTextureFromSurface(gRenderer, surface);
    if(SDLTexture == nullptr) {
        handleSDLError("Failed to create texture from surface");
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

    animation = std::vector<int>{0};
}

void TA_Sprite::draw()
{
    if(animation.size() >= 2) {
        animationTimer += gElapsedTime;
        animationFrame += animationTimer / animationDelay;

        if (animationFrame >= animation.size()) {
            if (repeatTimesLeft != -1) {
                repeatTimesLeft -= animationFrame / animation.size();
                if (repeatTimesLeft <= 0) {
                    setFrame(animation.back());
                    frame = animation[0];
                }
            }
            animationFrame %= animation.size();
        }

        animationTimer = std::fmod(animationTimer, animationDelay);
        frame = animation[animationFrame];
    }
    else{
        frame = animation[0];
    }

    SDL_Rect srcRect, dstRect;
    srcRect.x = (frameWidth * frame) % texture->width;
    srcRect.y = (frameWidth * frame) / texture->width * frameHeight;
    srcRect.w = frameWidth;
    srcRect.h = frameHeight;

    TA_Point screenPosition = position;
    if(camera != nullptr) {
        screenPosition = camera->getRelative(position);
    }

    dstRect.x = screenPosition.x * gWidthMultiplier;
    dstRect.y = screenPosition.y * gHeightMultiplier;
    dstRect.w = frameWidth * gWidthMultiplier * scale.x + 1;
    dstRect.h = frameHeight * gHeightMultiplier * scale.y + 1;
    SDL_RenderCopyEx(gRenderer, texture->SDLTexture, &srcRect, &dstRect, 0, nullptr, SDL_FLIP_NONE);
}

void TA_Sprite::setAnimation(std::vector<int> newAnimation, int newAnimationDelay, int repeatTimes)
{
    animation = newAnimation;
    animationDelay = newAnimationDelay;
    animationFrame = animationTimer = 0;
    repeatTimesLeft = repeatTimes;
}

void TA_Sprite::setAnimation(int firstFrame, int lastFrame, int newAnimationDelay, int repeatTimes)
{
    std::vector<int> newAnimation(lastFrame - firstFrame + 1);
    std::iota(newAnimation.begin(), newAnimation.end(), firstFrame);
    setAnimation(newAnimation, newAnimationDelay, repeatTimes);
}

void TA_Sprite::setFrame(int newFrame)
{
    setAnimation(newFrame, newFrame, 1, -1);
}

bool TA_Sprite::isAnimated()
{
    return animation.size() >= 2;
}

void TA_Sprite::setAlpha(int alpha)
{
    alpha = std::min(alpha, 255);
    alpha = std::max(alpha, 0);
    SDL_SetTextureAlphaMod(texture->SDLTexture, alpha);
}

TA_Sprite::~TA_Sprite()
{
    texture->spritesUsed --;
    if(texture->spritesUsed == 0) {
        delete texture;
    }
}
