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
protected:
    const std::regex key_regex;
public:
    const handler action;

    KeyBinding(const std::regex &key_rgx, handler fn);
    
    bool match(const std::string &test_seq) const;
};

class OptKeyBinding : public KeyBinding
{
public:
    const std::vector<InputOption> options;

    OptKeyBinding(const std::regex &key_rgx, InputOption opt, handler fn);
    OptKeyBinding(const std::regex &key_rgx, const std::vector<InputOption> &opts, handler fn);

    bool match(const std::string &test_seq, const std::vector<InputOption> &opts) const;

};

} // namespace bashkir
