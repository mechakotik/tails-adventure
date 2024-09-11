#ifndef TA_FILESYSTEM_H
#define TA_FILESYSTEM_H

#include <string>

namespace TA::filesystem {
    void fixPath(std::string &path);
    bool fileExists(std::string path);
    std::string readFile(std::string path);
    std::string readAsset(std::string path);
    std::string getAssetsPath();
    std::string getExecutableDirectory();
    void writeFile(std::string path, std::string value);
}

#endif // TA_FILESYSTEM_H
