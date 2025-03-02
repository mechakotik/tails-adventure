#include "resource_manager.h"
#include <unordered_map>
#include "SDL3_image/SDL_image.h"
#include "error.h"
#include "filesystem.h"
#include "tools.h"

namespace TA {
    namespace resmgr {
        std::unordered_map<std::filesystem::path, SDL_Texture*> textureMap;
        std::unordered_map<std::filesystem::path, Mix_Music*> musicMap;
        std::unordered_map<std::filesystem::path, Mix_Chunk*> chunkMap;
        std::unordered_map<std::filesystem::path, std::string> assetMap;
        std::unordered_map<std::filesystem::path, toml::value> tomlMap;

        void preloadTextures();
        void preloadChunks();
    }
}

void TA::resmgr::preload() {
    preloadTextures();
    preloadChunks();
}

void TA::resmgr::preloadTextures() {
    const std::vector<std::string> names{"bomb", "enemy_bomb", "enemy_rock", "explosion", "leaf", "nezu_bomb", "ring",
        "rock", "splash", "walker_bullet"};

    for(std::string name : names) {
        loadTexture("objects/" + name + ".png");
    }
}

void TA::resmgr::preloadChunks() {
    const std::vector<std::string> names{"break", "damage", "enter", "explosion", "fall", "find_item", "fly", "hammer",
        "hit", "item_switch", "jump", "land", "open", "remote_robot_fly", "remote_robot_step", "ring", "select_item",
        "select", "shoot", "switch", "teleport"};

    for(std::string name : names) {
        loadChunk("sound/" + name + ".ogg");
    }
}

SDL_Texture* TA::resmgr::loadTexture(std::filesystem::path path) {
    path = TA::filesystem::getAssetsPath() / path;

    if(!textureMap.count(path)) {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if(surface == nullptr) {
            TA::handleSDLError("%s", "Failed to load image");
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(TA::renderer, surface);
        if(texture == nullptr) {
            TA::handleSDLError("%s", "Failed to create texture from surface");
        }
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
        textureMap[path] = texture;
        SDL_DestroySurface(surface);
    }

    return textureMap[path];
}

Mix_Music* TA::resmgr::loadMusic(std::filesystem::path path) {
    path = TA::filesystem::getAssetsPath() /  path;

    if(!musicMap.count(path)) {
        musicMap[path] = Mix_LoadMUS(path.c_str());
        if(musicMap[path] == nullptr) {
            TA::handleSDLError("%s load failed", path.c_str());
        }
    }

    return musicMap[path];
}

Mix_Chunk* TA::resmgr::loadChunk(std::filesystem::path path) {
    path = TA::filesystem::getAssetsPath() / path;

    if(!chunkMap.contains(path)) {
        chunkMap[path] = Mix_LoadWAV(path.c_str());
        if(chunkMap[path] == nullptr) {
            TA::handleSDLError("%s load failed", path.c_str());
        }
    }

    return chunkMap[path];
}

const std::string& TA::resmgr::loadAsset(std::filesystem::path path) {
    path = TA::filesystem::getAssetsPath() / path;
    if(!assetMap.contains(path)) {
        assetMap[path] = TA::filesystem::readFile(path);
    }
    return assetMap[path];
}

const toml::value& TA::resmgr::loadToml(std::filesystem::path path) {
    path = TA::filesystem::getAssetsPath() / path;
    if(!tomlMap.contains(path)) {
        try {
            tomlMap[path] = toml::parse_str(TA::filesystem::readFile(path));
        } catch(std::exception& e) {
            TA::handleError("Failed to load %s:\n%s", path, e.what());
        }
    }
    return tomlMap[path];
}

void TA::resmgr::quit() {
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
