#pragma once
#include "deps.h"
// #include <algorithm>
// #include <numeric>
// #include <vector>
// #include <string>
// #include <sstream>
// #include <string.h>
#include "input/InputHandler.h"

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

inline bool endswith(const std::string &str, const std::string &ending)
{
    if (str.length() > ending.length())
    {
        return str.compare(str.length() - ending.length(), ending.length(), ending) == 0;
    }
    else
    {
        return false;
    }
}

inline std::unique_ptr<char[]> substr(const char *str, size_t start_pos, size_t length)
{
    std::unique_ptr<char[]> substr = std::make_unique<char[]>(length + 1);
    memcpy(substr.get(), &str[start_pos], length);
    substr[length] = '\0';
    return substr;
}

inline std::string substr(const std::string &str, size_t start_pos, size_t length)
{
    return str.substr(start_pos, length);
}

inline std::string substr(const std::string &str, size_t start_pos)
{
    return str.substr(start_pos, str.length() - start_pos);
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

inline std::tuple<std::string, std::string> splitInHalf(const std::string &base, size_t pos)
{
    if (pos > base.length())
    {
        throw std::invalid_argument("Splitting position greater than string length");
    }
    else
    {
        std::string part_1 = base.substr(0, pos);
        std::string part_2 = pos < base.length() ? base.substr(pos) : "";
        return std::make_tuple(part_1, part_2);
    }
}

inline std::string join(const std::vector<std::string> &strs, const std::string &between)
{
    if (strs.size() == 0)
    {
        return "";
    }
    std::stringstream ss;
    ss << strs[0];
    for (std::size_t i = 1; i < strs.size(); ++i)
    {
        ss << between << strs[i];
    }
    return ss.str();
}

inline std::string join(const std::vector<Line> &strs, const char *between)
{
    if (strs.size() == 0)
    {
        return "";
    }
    std::stringstream ss;
    ss << strs[0].data;
    for (std::size_t i = 1; i < strs.size(); ++i)
    {
        ss << between << strs[i].data;
    }
    return ss.str();
}

inline bool remove_eol(std::string &str)
{
    const std::vector<std::string> crlf = {"\n", "\r"};
    size_t k = 0, cut_size = 0;
    bool positive_loop, has_changed = false;
    do
    {
        positive_loop = false;
        for (const std::string &eol : crlf)
        {
            if (endswith(str, eol))
            {
                str = str.substr(0, str.length() - eol.length());
                has_changed = positive_loop = true;
                break;
            }
        }
    } while (positive_loop);
    return has_changed;
}

inline bool isSubstrBeforePos(const char *fullstr, const char *substr, size_t pos)
{
    size_t len_f = strlen(fullstr),
           len_s = strlen(substr);
    if (pos > len_f || static_cast<int>(pos) - static_cast<int>(pos - len_s) < 0)
    {
        return false;
    }
    for (size_t i = 0; i < len_s; ++i)
    {
        if (fullstr[pos - len_s + i] != substr[i])
        {
            return false;
        }
    }
    return true;
}

} // namespace bashkir::util
