#pragma once
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>

namespace bashkir::util
{

inline std::string &ltrim(std::string &str)
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
                  return !std::isspace(ch);
              }));
    return str;
}

inline std::string &rtrim(std::string &str)
{
    str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) {
                  return !std::isspace(ch);
              })
                  .base(),
              str.end());
    return str;
}

inline void trim(std::string &str)
{
    ltrim(str);
    rtrim(str);
}

inline bool startswith(const std::string &str, const std::string &&prefix)
{
    return str.rfind(prefix) == 0;
}

inline bool startswith(const std::string &str, const std::string &prefix)
{
    return str.rfind(prefix) == 0;
}

inline char* substr(const char *str, std::size_t start_pos, std::size_t length)
{
    char *substr = new char[length + 1];
    memcpy(substr, &str[start_pos], length);
    substr[length] = '\0';
    return substr;
}

inline std::vector<std::string> split(const std::string &base, const char delim)
{
    std::stringstream sstream(base);
    std::vector<std::string> parts;
    std::string next_part;
    while (std::getline(sstream, next_part, delim))
    {
        parts.push_back(next_part);
    }
    return parts;
}

inline std::tuple<std::string, std::string> splitInHalf(const std::string &base, const char delim)
{
    std::size_t split_pos = base.find(delim);
    if (split_pos == std::string::npos)
    {
        return std::make_tuple(base, "");
    }
    else
    {
        std::string part_1 = base.substr(0, split_pos);
        std::string part_2 = base.substr(split_pos + 1);
        return std::make_tuple(part_1, part_2);
    }
}

inline std::string join(const std::vector<std::string> &strs, const std::string &between)
{
    if (strs.size() == 0)
    {
        return "";
    }
    else if (strs.size() == 1)
    {
        return strs[0];
    }
    std::stringstream ss;
    ss << strs[0];
    for (std::size_t i = 1; i < strs.size(); ++i)
    {
        ss << between << strs[i];
    }
    return ss.str();
}

} // namespace bashkir::util
