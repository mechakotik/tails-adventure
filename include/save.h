#ifndef TA_SAVE_H
#define TA_SAVE_H

#include <string>

namespace TA { namespace save {
    void load();
    void writeToFile();
    long long getParameter(std::string name);
    void setParameter(std::string name, long long value);
    void setCurrentSave(std::string name);
    long long getSaveParameter(std::string name, std::string saveName = "");
    void setSaveParameter(std::string name, long long value, std::string saveName = "");
    void createSave(std::string saveName);
    void repairSave(std::string saveName);
    bool saveExists(int save);
}}

#endif // TA_SAVE_H
