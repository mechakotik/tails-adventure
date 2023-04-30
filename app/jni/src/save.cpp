#include <map>
#include <sstream>
#include "save.h"
#include "tools.h"
#include "error.h"

namespace TA { namespace save {
    void addOptionsFromFile(std::string filename);
    std::map<std::string, int> saveMap;
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
