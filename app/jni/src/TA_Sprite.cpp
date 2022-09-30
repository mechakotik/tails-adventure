#include <vector>
#include <numeric>
#include <algorithm>
#include "TA_Sprite.h"
#include "TA_Globals.h"
#include "TA_Error.h"

void TA_Sprite::load(std::string filename, int newFrameWidth, int newFrameHeight)
{
    TA_Texture::load(filename.c_str());

    if(newFrameWidth == -1){
        frameWidth = TA_Texture::textureWidth;
        frameHeight = TA_Texture::textureHeight;
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

        animationTimer %= animationDelay;
        frame = animation[animationFrame];
    }
    else{
        frame = animation[0];
    }

    SDL_Rect srcRect, dstRect;
    srcRect.x = (frameWidth * frame) % textureWidth;
    srcRect.y = (frameWidth * frame) / textureWidth * frameHeight;
    srcRect.w = frameWidth;
    srcRect.h = frameHeight;

    dstRect.x = xpos * gWidthMultiplier;
    dstRect.y = ypos * gHeightMultiplier;
    dstRect.w = frameWidth * gWidthMultiplier;
    dstRect.h = frameHeight * gHeightMultiplier;
    SDL_RenderCopyEx(gRenderer, texture, &srcRect, &dstRect, 0, nullptr, SDL_FLIP_NONE);
}

void TA_Sprite::setPosition(double newXpos, double newYpos)
{
    xpos = newXpos;
    ypos = newYpos;
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
    printLog("%i", alpha);
    alpha = std::min(alpha, 255);
    alpha = std::max(alpha, 0);
    SDL_SetTextureAlphaMod(TA_Texture::texture, alpha);
}
