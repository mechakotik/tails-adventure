#include <map>
#include <fstream>
#include <filesystem>
#include "save.h"
#include "tools.h"
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
        "item_position"
    };
}}

void TA::save::addOptionsFromFile(std::string filename)
{
    std::ifstream fin(filename);
    if(!fin.is_open()) {
        return;
    }

    std::string name;
    long long value;
    while(fin >> name >> value) {
        saveMap[name] = value;
    }
    fin.close();
}

void TA::save::load()
{
    std::string defaultConfigPath = "default_config";
    TA::addPathPrefix(defaultConfigPath);
    addOptionsFromFile(defaultConfigPath);
    addOptionsFromFile(getSaveFileName());
}

void TA::save::writeToFile()
{
    std::ofstream fout(getSaveFileName());
    if(!fout.is_open()) {
        return;
    }

    for(auto [key, value] : saveMap) {
        fout << key << ' ' << value << std::endl;
    }
    fout.close();
}

std::string TA::save::getSaveFileName()
{
    #ifdef TA_LINUX_INSTALL
        std::string path = std::string(getenv("HOME")) + "/.local/share/tails-adventure/";
        std::filesystem::create_directories(path);
        return path + "config";
    #else
        return "config";
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
