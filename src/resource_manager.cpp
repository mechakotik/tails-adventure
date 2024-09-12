#include <unordered_map>
#include "SDL3_image/SDL_image.h"
#include "resource_manager.h"
#include "error.h"
#include "filesystem.h"
#include "tools.h"

namespace TA { namespace resmgr {
    std::unordered_map<std::string, SDL_Texture*> textureMap;
    std::unordered_map<std::string, Mix_Music*> musicMap;
    std::unordered_map<std::string, Mix_Chunk*> chunkMap;
    std::unordered_map<std::string, std::string> assetMap;

    void preloadTextures();
    void preloadChunks();
}}

void TA::resmgr::preload()
{
    preloadTextures();
    preloadChunks();
}

void TA::resmgr::preloadTextures()
{
    const std::vector<std::string> names {
        "bomb",
        "enemy_bomb",
        "enemy_rock",
        "explosion",
        "leaf",
        "nezu_bomb",
        "ring",
        "rock",
        "splash",
        "walker_bullet"
    };

    for(std::string name : names) {
        loadTexture("objects/" + name + ".png");
    }
}

void TA::resmgr::preloadChunks()
{
    const std::vector<std::string> names {
        "break",
        "damage",
        "enter",
        "explosion",
        "fall",
        "find_item",
        "fly",
        "hammer",
        "hit",
        "item_switch",
        "jump",
        "land",
        "open",
        "remote_robot_fly",
        "remote_robot_step",
        "ring",
        "select_item",
        "select",
        "shoot",
        "switch",
        "teleport"
    };

    for(std::string name : names) {
        loadChunk("sound/" + name + ".ogg");
    }
}

SDL_Texture* TA::resmgr::loadTexture(std::string filename)
{
    filename = TA::filesystem::getAssetsPath() + "/" + filename;
    TA::filesystem::fixPath(filename);

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
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
        textureMap[filename] = texture;
        SDL_DestroySurface(surface);
    }

    return textureMap[filename];
}

Mix_Music* TA::resmgr::loadMusic(std::string filename)
{
    filename = TA::filesystem::getAssetsPath() + "/" + filename;
    TA::filesystem::fixPath(filename);

    if(!musicMap.count(filename)) {
        musicMap[filename] = Mix_LoadMUS(filename.c_str());
        if(musicMap[filename] == nullptr) {
            TA::handleSDLError("%s load failed", filename.c_str());
        }
    }

    return musicMap[filename];
}

Mix_Chunk* TA::resmgr::loadChunk(std::string filename)
{
    filename = TA::filesystem::getAssetsPath() + "/" + filename;
    TA::filesystem::fixPath(filename);

    if(!chunkMap.count(filename)) {
        chunkMap[filename] = Mix_LoadWAV(filename.c_str());
        if(chunkMap[filename] == nullptr) {
            TA::handleSDLError("%s load failed", filename.c_str());
        }
    }

    return chunkMap[filename];
}

std::string TA::resmgr::loadAsset(std::string filename)
{
    filename = TA::filesystem::getAssetsPath() + "/" + filename;
    TA::filesystem::fixPath(filename);

    if(!assetMap.count(filename)) {
        assetMap[filename] = TA::filesystem::readFile(filename);
    }

    return assetMap[filename];
}

void TA::resmgr::quit()
{
    for(std::pair<std::string, SDL_Texture*> element : textureMap) {
        SDL_DestroyTexture(element.second);
    }
    for(std::pair<std::string, Mix_Music*> element : musicMap) {
        Mix_FreeMusic(element.second);
    }
    for(std::pair<std::string, Mix_Chunk*> element : chunkMap) {
        Mix_FreeChunk(element.second);
    }
}
