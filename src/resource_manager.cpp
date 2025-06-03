#include "resource_manager.h"
#include <unordered_map>
#include "SDL3_image/SDL_image.h"
#include "error.h"
#include "filesystem.h"
#include "tools.h"

namespace TA::resmgr {
    struct Mod {
        std::filesystem::path root;
        std::vector<std::filesystem::path> files;
        int priority = 0;
        bool enabled = false;
    };

    void loadMods();
    Mod loadMod(std::filesystem::path root);
    std::filesystem::path getAssetPath(std::filesystem::path asset);

    void preloadTextures();
    void preloadChunks();

    std::unordered_map<std::string, std::filesystem::path> overrides;
    int totalMods = 0;
    int loadedMods = 0;

    std::unordered_map<std::string, SDL_Texture*> textureMap;
    std::unordered_map<std::string, Mix_Music*> musicMap;
    std::unordered_map<std::string, Mix_Chunk*> chunkMap;
    std::unordered_map<std::string, std::string> assetMap;
    std::unordered_map<std::string, toml::value> tomlMap;
}

void TA::resmgr::load() {
    loadMods();
    preloadTextures();
    preloadChunks();
}

void TA::resmgr::loadMods() {
#ifdef __ANDROID__
    if(SDL_GetAndroidExternalStorageState() !=
        (SDL_ANDROID_EXTERNAL_STORAGE_READ | SDL_ANDROID_EXTERNAL_STORAGE_WRITE)) {
        return;
    }
    std::filesystem::path modsPath = SDL_GetAndroidExternalStoragePath();
    modsPath /= "mods";
#elifdef TA_UNIX_INSTALL
    std::filesystem::path modsPath = "~/.local/share/tails-adventure/mods";
#else
    std::filesystem::path modsPath = TA::filesystem::getExecutableDirectory() / "mods";
#endif

    if(!std::filesystem::is_directory(modsPath)) {
        return;
    }

    std::vector<Mod> mods;
    for(const auto& mod : std::filesystem::directory_iterator(modsPath)) {
        if(std::filesystem::exists(mod.path()) && std::filesystem::is_directory(mod.path())) {
            mods.push_back(loadMod(mod.path()));
        }
    }

    std::sort(mods.begin(), mods.end(), [](const Mod& a, const Mod& b) { return a.priority < b.priority; });

    std::vector<std::string> loaded;
    for(const Mod& mod : mods) {
        totalMods++;
        if(!mod.enabled) continue;
        loaded.push_back(mod.root.filename().string());
        loadedMods++;
        for(const std::filesystem::path& path : mod.files) {
            std::string rel = std::filesystem::relative(path, mod.root).string();
            overrides[rel] = path;
        }
    }

    if(!loaded.empty()) {
        std::string log = "loaded mods (highest priority last): ";
        for(const std::string name : loaded) {
            log += name;
            log += ' ';
        }
        TA::printLog("%s", log.c_str());
    }
}

TA::resmgr::Mod TA::resmgr::loadMod(std::filesystem::path root) {
    Mod mod = Mod();
    mod.root = root;

    if(!std::filesystem::is_regular_file(root / "enabled") ||
        TA::filesystem::readFile(root / "enabled").front() != '1') {
        mod.enabled = false;
        return mod;
    }

    mod.enabled = true;
    for(const auto& file : std::filesystem::recursive_directory_iterator(root)) {
        if(std::filesystem::is_regular_file(file.path())) {
            mod.files.emplace_back(file.path());
        }
    }
    if(std::filesystem::is_regular_file(root / "priority")) {
        mod.priority = std::stoi(TA::filesystem::readFile(root / "priority"));
    }

    return mod;
}

std::filesystem::path TA::resmgr::getAssetPath(std::filesystem::path asset) {
    if(overrides.contains(asset.string())) {
        return overrides.at(asset.string());
    }
    return TA::filesystem::getAssetsPath() / asset;
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
    path = getAssetPath(path.string());

    if(!textureMap.count(path.string())) {
        std::string pathStr = path.string();
        SDL_Surface* surface = IMG_Load(pathStr.c_str());
        if(surface == nullptr) {
            TA::handleSDLError("%s", "failed to load image");
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(TA::renderer, surface);
        if(texture == nullptr) {
            TA::handleSDLError("%s", "failed to create texture from surface");
        }
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
        textureMap[path.string()] = texture;
        SDL_DestroySurface(surface);
    }

    return textureMap[path.string()];
}

Mix_Music* TA::resmgr::loadMusic(std::filesystem::path path) {
    path = getAssetPath(path);

    if(!musicMap.count(path.string())) {
        std::string pathStr = path.string();
        musicMap[path.string()] = Mix_LoadMUS(pathStr.c_str());
        if(musicMap[path.string()] == nullptr) {
            TA::handleSDLError("%s load failed", pathStr.c_str());
        }
    }

    return musicMap[path.string()];
}

Mix_Chunk* TA::resmgr::loadChunk(std::filesystem::path path) {
    path = getAssetPath(path);

    if(!chunkMap.contains(path.string())) {
        std::string pathStr = path.string();
        chunkMap[path.string()] = Mix_LoadWAV(pathStr.c_str());
        if(chunkMap[path.string()] == nullptr) {
            TA::handleSDLError("%s load failed", pathStr.c_str());
        }
    }

    return chunkMap[path.string()];
}

const std::string& TA::resmgr::loadAsset(std::filesystem::path path) {
    path = getAssetPath(path);
    if(!assetMap.contains(path.string())) {
        assetMap[path.string()] = TA::filesystem::readFile(path);
    }
    return assetMap[path.string()];
}

const toml::value& TA::resmgr::loadToml(std::filesystem::path path) {
    path = getAssetPath(path);
    if(!tomlMap.contains(path.string())) {
        try {
            tomlMap[path.string()] = toml::parse_str(TA::filesystem::readFile(path));
        } catch(std::exception& e) {
            TA::handleError("failed to load %s\n%s", path.c_str(), e.what());
        }
    }
    return tomlMap[path.string()];
}

int TA::resmgr::getLoadedMods() {
    return loadedMods;
}

int TA::resmgr::getTotalMods() {
    return totalMods;
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
