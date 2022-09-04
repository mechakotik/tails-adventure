#include "TA_Sprite.h"
#include "TA_Globals.h"

void TA_Sprite::load(const char *filename, int newFrameWidth, int newFrameHeight)
{
    TA_Texture::load(filename);
    frameWidth = newFrameWidth;
    frameHeight = newFrameHeight;
}

void TA_Sprite::draw()
{
    animationTimer += gElapsedTime;
    animationFrame = (animationFrame + animationTimer / animationDelay) % animation.size();
    animationTimer %= animationDelay;
    frame = animation[animationFrame];

    SDL_Rect srcRect, dstRect;
    srcRect.x = (frameWidth * frame) % textureWidth;
    srcRect.y = (frameWidth * frame) / textureWidth * frameHeight;
    dstRect.x = xpos;
    dstRect.y = ypos;
    SDL_RenderCopyEx(gRenderer, texture, &srcRect, &dstRect, 0, nullptr, SDL_FLIP_NONE);
}

void TA_Sprite::setPosition(int newXpos, int newYpos)
{
    xpos = newXpos;
    ypos = newYpos;
}

void TA_Sprite::setAnimation(std::vector<int> newAnimation, int newAnimationDelay)
{
    animation = newAnimation;
    animationDelay = newAnimationDelay;
    animationFrame = animationTimer = 0;
}
