#ifndef TA_FILESYSTEM_H
#define TA_FILESYSTEM_H

#include <filesystem>

namespace TA::filesystem {
    bool fileExists(std::filesystem::path path);
    std::string readFile(std::filesystem::path path);
    std::string readAsset(std::filesystem::path path);
    std::filesystem::path getAssetsPath();
    std::filesystem::path getExecutableDirectory();
    void writeFile(std::filesystem::path path, std::string value);
}

#endif // TA_FILESYSTEM_H
