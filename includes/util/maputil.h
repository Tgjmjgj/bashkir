#pragma once
#include "deps.h"
// #include <map>
// #include <string>
// #include <optional>
#include "util/strutil.h"

namespace bashkir::util
{

// unused
template<typename Key, typename Val>
bool isKeyInMap(std::map<Key, Val> map, Key key)
{
    return map.find(key) != map.end();
}

// unused
template<typename Key, typename Val>
bool isValueInMap(std::map<Key, Val> map, Val value)
{
    for (auto& [key, val] : map)
    {
        if (val == value)
        {
            return true;
        }
    }
    return false;
}

// unused
std::optional<std::string> findKeyFromMapBeforePos(std::map<std::string, std::string> map, const char *str, size_t pos)
{
    if (pos > strlen(str))
    {
        return std::nullopt;
    }
    for (auto& [key, val] : map)
    {
        if (util::isSubstrBeforePos(str, key.c_str(), pos))
        {
            return key;
        }
    }
    return std::nullopt;
}

// unused
std::optional<std::string> findValueFromMapBeforePos(std::map<std::string, std::string> map, const char *str, size_t pos)
{
    if (pos > strlen(str))
    {
        return std::nullopt;
    }
    for (auto& [key, val] : map)
    {
        if (util::isSubstrBeforePos(str, val.c_str(), pos))
        {
            return key;
        }
    }
    return std::nullopt;
}

} // namespace bashkir::util
