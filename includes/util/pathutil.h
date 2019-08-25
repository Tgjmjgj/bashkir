#pragma once
#include <string>
#include "util/strutil.h"

namespace bashkir::util
{

inline std::string& fullToHomeRel(std::string &path)
{
    const std::string home_path = getenv("HOME");
    if (startswith(path, home_path))
    {
        path.replace(0, home_path.length(), "~");
    }
    return path;
}

inline std::string& homeRelToFull(std::string &path)
{
    const std::string home_path = getenv("HOME");
    if (path[0] == '~')
    {
        path.replace(0, 1, home_path);
    }
    return path;
}

} // namespace bashkir::util
