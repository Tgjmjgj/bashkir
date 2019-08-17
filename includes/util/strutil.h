#pragma once
#include <algorithm>
#include <string>

namespace util
{
    inline void ltrim(std::string &str)
    {
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
            return !std::isspace(ch);
        }));
    }

    inline void rtrim(std::string &str)
    {
        str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), str.end());
    }

    inline void trim(std::string &str)
    {
        ltrim(str);
        rtrim(str);
    }
};
