#include <unordered_map>
#include <SDL_image.h>
#include "resource_manager.h"
#include "error.h"
#include "tools.h"

namespace TA { namespace resmgr {
    std::unordered_map<std::string, SDL_Texture*> textureMap;
    std::unordered_map<std::string, Mix_Chunk*> chunkMap;
}}

SDL_Texture* TA::resmgr::loadTexture(std::string filename)
{
    TA::addPathPrefix(filename);
    if(!textureMap.count(filename)) {
        SDL_Surface *surface = IMG_Load(filename.c_str());
        if(surface == nullptr) {
            TA::handleSDLError("%s", "Failed to load image");
        }
        SDL_Texture *texture = SDL_CreateTextureFromSurface(TA::renderer, surface);
        if(texture == nullptr) {
            TA::handleSDLError("%s", "Failed to create texture from surface");
        }
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        textureMap[filename] = texture;
        SDL_FreeSurface(surface);
    }
    return textureMap[filename];
}

Mix_Chunk* TA::resmgr::loadChunk(std::string filename)
{
    TA::addPathPrefix(filename);
    if(!chunkMap.count(filename)) {
        chunkMap[filename] = Mix_LoadWAV(filename.c_str());
        if(chunkMap[filename] == nullptr) {
            TA::handleSDLError("%s load failed", filename.c_str());
        }
    }
    return chunkMap[filename];
}

void TA::resmgr::quit()
{
    for(std::pair<std::string, SDL_Texture*> element : textureMap) {
        SDL_DestroyTexture(element.second);
    }
    for(std::pair<std::string, Mix_Chunk*> element : chunkMap) {
        Mix_FreeChunk(element.second);
    }
}
