#pragma once
#include <string>
#include <vector>
#include <regex>
#include <experimental/filesystem>
#include "util/strutil.h"

namespace fs = std::experimental::filesystem;

namespace bashkir::util
{

inline std::string &fullToHomeRel(std::string &path)
{
    const std::string home_path = getenv("HOME");
    if (startswith(path, home_path))
    {
        path.replace(0, home_path.length(), "~");
    }
    return path;
}

inline bool tryHomeRelToFull(std::string &path)
{
    const std::string home_path = getenv("HOME");
    if (path == "~")
    {
        path.replace(0, 1, home_path);
        return true;
    }
    else
    {
        std::regex re("~/(?:(?:(?:[^/\n])+/)*(?:(?:[^/\n])+)?)?");
        if (std::regex_match(path, re))
        {
            path.replace(0, 1, home_path);
            return true;
        }
        else
        {
            return false;
        }
    }
}

inline std::string resolveFileName(const std::string &name)
{
    if (name.empty())
    {
        return std::string();
    }
    std::string cpname = std::string(name);
    if (tryHomeRelToFull(cpname))
    {
        if (fs::is_regular_file(cpname))
        {
            return cpname;
        }
        else
        {
            return std::string();
        }
    }
    if (name[0] == '/')
    {
        if (fs::is_regular_file(name))
        {
            return name;
        }
        else
        {
            return std::string();
        }
    }
    const std::string path(getenv("PATH"));
    std::vector<std::string> splits = util::split(path, ':');
    for (std::string &str : splits)
    {
        const std::string try_full_name = str + "/" + name;
        if (fs::is_regular_file(try_full_name))
        {
            return try_full_name;
        }
    }
    return std::string();
}

} // namespace bashkir::util
