#pragma once
#include <string>

namespace bashkir::util
{

inline std::string& fullToHomeRel(std::string &path)
{
    const std::string home_path = getenv("HOME");
    if (path.rfind(home_path) == 0)
    {
        path.replace(0, home_path.length(), "~");
    }
    return path;
}

inline std::string& homeRelToFull(std::string &&path)
{
    const std::string home_path = getenv("HOME");
    if (path[0] == '~')
    {
        path.replace(0, 1, home_path);
    }
    return path;
}

} // namespace bashkir::util
