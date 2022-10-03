#include "texture.h"
#include "error.h"
#include "globals.h"

void TA_Texture::load(const char *filename)
{
    SDL_Surface *surface = IMG_Load(filename);
    if(surface == nullptr){
        handleSDLError("Failed to load image");
    }

    texture = SDL_CreateTextureFromSurface(gRenderer, surface);
    if(texture == nullptr){
        handleSDLError("Failed to create texture from surface");
    }
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    textureWidth = surface->w;
    textureHeight = surface->h;
    SDL_FreeSurface(surface);
}

TA_Texture::~TA_Texture()
{
    SDL_DestroyTexture(texture);
}
