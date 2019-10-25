#pragma once
#include "deps.h"
#include "input/InputSpecialKey.h"
#include "input/InputActions.h"
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
    std::map<InputSpecialKey, handler> key_map;

public:
    KeyBindingsMap(InputHandler &owner);
    std::optional<handler> get(const InputSpecialKey &key) const;
private:
    void bindKeys();
};

} // namespace bashkir
