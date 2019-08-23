#pragma once
#include <algorithm>
#include <string>

namespace bashkir::util
{

inline std::string& ltrim(std::string &str)
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
        return !std::isspace(ch);
    }));
    return str;
}

inline std::string& rtrim(std::string &str)
{
    str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), str.end());
    return str;
}

inline void trim(std::string &str)
{
    ltrim(str);
    rtrim(str);
}

} // namespace bashkir::util
