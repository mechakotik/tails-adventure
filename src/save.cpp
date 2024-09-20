#include <map>
#include <sstream>
#include <filesystem>
#include <vector>
#include "save.h"
#include "filesystem.h"
#include "error.h"

namespace TA { namespace save {
    void addOptionsFromFile(std::string filename);
    std::string getSaveFileName();
    std::map<std::string, long long> saveMap;
    std::string currentSave = "";

    const std::vector<std::string> latestNames {
        "item_mask",
        "boss_mask",
        "rings",
        "item_slot0",
        "item_slot1",
        "item_slot2",
        "item_slot3",
        "item_position",
        "seafox_item_slot0",
        "seafox_item_slot1",
        "seafox_item_slot2",
        "seafox_item_slot3",
        "seafox_item_position"
    };
}}

void TA::save::addOptionsFromFile(std::string path)
{
    if(!TA::filesystem::fileExists(path)) {
        TA::printWarning("Save file %s was not found, skipping", path.c_str());
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

void TA::save::load()
{
    std::string defaultConfigPath = TA::filesystem::getAssetsPath() + "/default_config";
    addOptionsFromFile(defaultConfigPath);
    addOptionsFromFile(getSaveFileName());
}

void TA::save::writeToFile()
{
    std::stringstream output;
    for(auto [key, value] : saveMap) {
        output << key << ' ' << value << std::endl;
    }

    std::string name = getSaveFileName();
    TA::filesystem::writeFile(name, output.str());
}

std::string TA::save::getSaveFileName()
{
    #ifdef __ANDROID__
        std::string path = SDL_GetAndroidInternalStoragePath();
        return path + "/config";
    #else
        #ifdef TA_UNIX_INSTALL
            std::string path = std::string(getenv("HOME")) + "/.local/share/tails-adventure";
            std::filesystem::create_directories(path);
            return path + "/config";
        #else
            return TA::filesystem::getExecutableDirectory() + "/config";
        #endif
    #endif
}

long long TA::save::getParameter(std::string name)
{
    if(!saveMap.count(name)) {
        TA::handleError("Unknown parameter %s", name.c_str());
    }
    return saveMap[name];
}

void TA::save::setParameter(std::string name, long long value)
{
    saveMap[name] = value;
}

void TA::save::setCurrentSave(std::string name)
{
    currentSave = name;
}

long long TA::save::getSaveParameter(std::string name, std::string saveName)
{
    if(saveName == "") {
        saveName = currentSave;
    }
    return getParameter(saveName + "/" + name);
}

void TA::save::setSaveParameter(std::string name, long long value, std::string saveName)
{
    if(saveName == "") {
        saveName = currentSave;
    }
    setParameter(saveName + "/" + name, value);
}

void TA::save::createSave(std::string saveName)
{
    std::map<std::string, long long> newSaveMap = saveMap;
    const std::string defaultSaveName = "default_save/";

    for(auto item : saveMap) {
        if(item.first.length() >= defaultSaveName.length() && item.first.substr(0, defaultSaveName.length()) == defaultSaveName) {
            std::string itemName = saveName + "/" + item.first.substr(defaultSaveName.length(), item.first.length() - defaultSaveName.length());
            newSaveMap[itemName] = item.second;
        }
    }

    saveMap = newSaveMap;
}

void TA::save::repairSave(std::string saveName)
{
    std::map<std::string, long long> newSaveMap = saveMap;
    const std::string defaultSaveName = "default_save/";

    for(auto item : saveMap) {
        if(item.first.length() >= defaultSaveName.length() && item.first.substr(0, defaultSaveName.length()) == defaultSaveName) {
            std::string itemName = saveName + "/" + item.first.substr(defaultSaveName.length(), item.first.length() - defaultSaveName.length());
            if(!newSaveMap.count(itemName)) {
                newSaveMap[itemName] = item.second;
            }
        }
    }

    saveMap = newSaveMap;
}

void TA::save::saveToLatest()
{
    for(std::string name : latestNames) {
        setSaveParameter("latest_" + name, getSaveParameter(name));
    }
}

void TA::save::loadFromLatest()
{
    for(std::string name : latestNames) {
        setSaveParameter(name, getSaveParameter("latest_" + name));
    }
}

bool TA::save::saveExists(int save)
{
    std::string saveName = "save_" + std::to_string(save);
    return saveMap.count(saveName + "/item_mask");
}
