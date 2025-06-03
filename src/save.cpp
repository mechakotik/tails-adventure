#include "save.h"
#include <filesystem>
#include <map>
#include <sstream>
#include <vector>
#include "error.h"
#include "filesystem.h"

namespace TA {
    namespace save {
        void addOptionsFromFile(std::filesystem::path path);
        std::filesystem::path getSaveFileName();
        std::map<std::string, long long> saveMap;
        std::string currentSave = "";
    }
}

void TA::save::load() {
    std::filesystem::path defaultConfigPath = TA::filesystem::getAssetsPath() / "default_config";
    addOptionsFromFile(defaultConfigPath);
    addOptionsFromFile(getSaveFileName());
}

void TA::save::addOptionsFromFile(std::filesystem::path path) {
    if(!TA::filesystem::fileExists(path)) {
        TA::printWarning("save file %s was not found, skipping", path.c_str());
        return;
    }

    std::string options = TA::filesystem::readFile(path);
    std::stringstream stream;
    stream << options;

    std::string name;
    long long value;
    while(stream >> name >> value) {
        saveMap[name] = value;
    }
}

void TA::save::writeToFile() {
    std::stringstream output;
    for(auto [key, value] : saveMap) {
        output << key << ' ' << value << std::endl;
    }

    std::filesystem::path name = getSaveFileName();
    TA::filesystem::writeFile(name, output.str());
}

std::filesystem::path TA::save::getSaveFileName() {
#ifdef __ANDROID__
    std::filesystem::path internalPath = SDL_GetAndroidInternalStoragePath();
    internalPath /= "config";

    bool useExternal = (SDL_GetAndroidExternalStorageState() ==
                        (SDL_ANDROID_EXTERNAL_STORAGE_READ | SDL_ANDROID_EXTERNAL_STORAGE_WRITE));
    useExternal &= !TA::filesystem::fileExists(internalPath);

    if(useExternal) {
        return std::filesystem::path(SDL_GetAndroidExternalStoragePath()) / "config";
    }
    return internalPath;
#elifdef TA_UNIX_INSTALL
    std::filesystem::path path = std::filesystem::path(getenv("HOME")) / ".local/share/tails-adventure";
    std::filesystem::create_directories(path);
    return path / "config";
#else
    return TA::filesystem::getExecutableDirectory() / "config";
#endif
}

long long TA::save::getParameter(std::string name) {
    if(!saveMap.contains(name)) {
        TA::handleError("unknown parameter %s", name.c_str());
    }
    return saveMap[name];
}

void TA::save::setParameter(std::string name, long long value) {
    saveMap[name] = value;
}

void TA::save::setCurrentSave(std::string name) {
    currentSave = name;
}

long long TA::save::getSaveParameter(std::string name, std::string saveName) {
    if(saveName == "") {
        saveName = currentSave;
    }
    return getParameter(saveName + "/" + name);
}

void TA::save::setSaveParameter(std::string name, long long value, std::string saveName) {
    if(saveName == "") {
        saveName = currentSave;
    }
    setParameter(saveName + "/" + name, value);
}

void TA::save::createSave(std::string saveName) {
    std::map<std::string, long long> newSaveMap = saveMap;
    const std::string defaultSaveName = "default_save/";

    for(auto item : saveMap) {
        if(item.first.length() >= defaultSaveName.length() &&
            item.first.substr(0, defaultSaveName.length()) == defaultSaveName) {
            std::string itemName =
                saveName + "/" +
                item.first.substr(defaultSaveName.length(), item.first.length() - defaultSaveName.length());
            newSaveMap[itemName] = item.second;
        }
    }

    saveMap = newSaveMap;
}

void TA::save::repairSave(std::string saveName) {
    std::map<std::string, long long> newSaveMap = saveMap;
    const std::string defaultSaveName = "default_save/";

    for(auto item : saveMap) {
        if(item.first.length() >= defaultSaveName.length() &&
            item.first.substr(0, defaultSaveName.length()) == defaultSaveName) {
            std::string itemName =
                saveName + "/" +
                item.first.substr(defaultSaveName.length(), item.first.length() - defaultSaveName.length());
            if(!newSaveMap.count(itemName)) {
                newSaveMap[itemName] = item.second;
            }
        }
    }

    saveMap = newSaveMap;
}

bool TA::save::saveExists(int save) {
    std::string saveName = "save_" + std::to_string(save);
    return saveMap.count(saveName + "/item_mask");
}
