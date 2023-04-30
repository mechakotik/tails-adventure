#ifndef TA_SAVE_H
#define TA_SAVE_H

#include <string>

namespace TA { namespace save {
    void load();
    int getParameter(std::string name);
    void setParameter(std::string name, int value);
}}

#endif // TA_SAVE_H
