#include "input/KeyBinding.h"
#include "input/CsiSequences.h"

namespace bashkir
{

KeyBinding::KeyBinding(const std::regex &key_rgx, KeyBinding::handler fn) 
    : key_regex(key_rgx), action(fn) {}

std::vector<InputOption> vec(InputOption opt)
{
    return { opt };
}

KeyBinding::KeyBinding(const std::regex &key_rgx, InputOption opt, KeyBinding::handler fn)
    : key_regex(key_rgx), options(vec(opt)), action(fn) {}

KeyBinding::KeyBinding(const std::regex &key_rgx, const std::vector<InputOption> &opts, KeyBinding::handler fn)
    : key_regex(key_rgx), options(opts), action(fn) {}

bool KeyBinding::match(const std::string &test_seq, const std::vector<InputOption> &opts) const
{
    if (this->options != opts)
    {
        return false;
    }
    return std::regex_match(test_seq, this->key_regex);
}

} // namespace bashkir
