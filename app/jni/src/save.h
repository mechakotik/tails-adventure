#ifndef TA_SAVE_H
#define TA_SAVE_H

#include <string>

namespace TA { namespace save {
    void load();
    int getParameter(std::string name);
    void setParameter(std::string name, int value);
    void setCurrentSave(std::string name);
    int getSaveParameter(std::string name, std::string saveName = "");
    void setSaveParameter(std::string name, int value, std::string saveName = "");
    void createSave(std::string saveName);
}}

#endif // TA_SAVE_H
