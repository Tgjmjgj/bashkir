#pragma once
#include <string>

namespace bashkir::utils
{

inline std::string& fullToHomeRel(std::string &path)
{
    std::string homePath = getenv("HOME");
    if (path.rfind(homePath) == 0)
        path.replace(0, homePath.length(), "~");
    return path;
}

inline std::string& homeRelToFull(std::string &path)
{
    std::string homePath = getenv("HOME");
    if (path[0] == '~')
        path.replace(0, 1, homePath);
    return path;
}

} // namespace bashkir::utils
