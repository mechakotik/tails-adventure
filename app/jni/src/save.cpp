#include <map>
#include <sstream>
#include "save.h"
#include "tools.h"
#include "error.h"

namespace TA { namespace save {
    void addOptionsFromFile(std::string filename);
    std::map<std::string, int> saveMap;
    std::string currentSave = "";
}}

void TA::save::addOptionsFromFile(std::string filename)
{
    std::stringstream stream; {
        std::string file = TA::readStringFromFile(filename);
        stream << file;
    }

    std::string name;
    int value;
    while(stream >> name >> value) {
        saveMap[name] = value;
    }
}

void TA::save::load()
{
    addOptionsFromFile("default_config");
}

int TA::save::getParameter(std::string name)
{
    if(!saveMap.count(name)) {
        TA::handleError("Unknown parameter %s", name.c_str());
    }
    return saveMap[name];
}

void TA::save::setParameter(std::string name, int value)
{
    saveMap[name] = value;
}

void TA::save::setCurrentSave(std::string name)
{
    currentSave = name;
}

int TA::save::getSaveParameter(std::string name, std::string saveName)
{
    if(saveName == "") {
        saveName = currentSave;
    }
    return getParameter(saveName + "/" + name);
}

void TA::save::setSaveParameter(std::string name, int value, std::string saveName)
{
    if(saveName == "") {
        saveName = currentSave;
    }
    setParameter(saveName + "/" + name, value);
}

void TA::save::createSave(std::string saveName)
{
    std::map<std::string, int> newSaveMap = saveMap;
    std::string defaultSaveName = "default_save/";
    for(auto item : saveMap) {
        if(item.first.length() >= defaultSaveName.length() && item.first.substr(0, defaultSaveName.length()) == defaultSaveName) {
            std::string itemName = saveName + "/" + item.first.substr(defaultSaveName.length(), item.first.length() - defaultSaveName.length());
            newSaveMap[itemName] = item.second;
        }
    }
    saveMap = newSaveMap;
}
