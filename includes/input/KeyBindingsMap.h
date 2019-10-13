#pragma once
#include "deps.h"
#include "input/InputActions.h"
// #include <string>
// #include <map>
// #include <optional>
// #include <functional>

namespace bashkir
{

class KeyBindingsMap
{
using handler = std::function<bool(void)>;
private:
    InputActions actions;
    std::map<char, handler> key_map;
    std::map<std::string, handler> seq_map;

public:
    KeyBindingsMap(InputHandler &owner);
    std::optional<handler> operator[](char key) const;
    std::optional<handler> operator[](const std::string &key) const;
private:
    void bindKeys();
};

} // namespace bashkir
