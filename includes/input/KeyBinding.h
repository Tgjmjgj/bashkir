#pragma once
#include "deps.h"
#include "input/interface/InputOption.h"
// #include <string>
// #include <set>
// #include <functional>
// #include <regex>

namespace bashkir
{

class KeyBinding
{
public:
using handler = std::function<bool(void)>;
private:
    const std::regex key_regex;
    const std::vector<InputOption> options;
public:
    const handler action;
public:
    KeyBinding(const std::regex &key_rgx, handler fn);
    KeyBinding(const std::regex &key_rgx, InputOption opt, handler fn);
    KeyBinding(const std::regex &key_rgx, const std::vector<InputOption> &opts, handler fn);
    
    bool match(const std::string &test_seq, const std::vector<InputOption> &opts) const;

    KeyBinding operator=(const KeyBinding& bind);
};

} // namespace bashkir
