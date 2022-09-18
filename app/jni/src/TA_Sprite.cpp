#include <numeric>
#include "TA_Sprite.h"
#include "TA_Globals.h"
#include "TA_Error.h"

void TA_Sprite::load(const char *filename, int newFrameWidth, int newFrameHeight)
{
    TA_Texture::load(filename);
    frameWidth = newFrameWidth;
    frameHeight = newFrameHeight;
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
