#include "input/KeyBinding.h"
#include "input/CsiSequences.h"

namespace bashkir
{

KeyBinding::KeyBinding(const std::regex &key_rgx, KeyBinding::handler fn) 
    : key_regex(key_rgx), action(fn) {}

bool KeyBinding::match(const std::string &test_seq) const
{
    return std::regex_match(test_seq, this->key_regex);
}

OptKeyBinding::OptKeyBinding(const std::regex &key_rgx, InputOption opt, KeyBinding::handler fn)
    : KeyBinding(key_rgx, fn), options({ opt }) {}

OptKeyBinding::OptKeyBinding(const std::regex &key_rgx, const std::vector<InputOption> &opts, KeyBinding::handler fn)
    : KeyBinding(key_rgx, fn), options(opts) {}

bool OptKeyBinding::match(const std::string &test_seq, const std::vector<InputOption> &opts) const
{
    if (!this->options.empty() && this->options != opts)
    {
        return false;
    }
    return std::regex_match(test_seq, this->key_regex);
}

} // namespace bashkir
