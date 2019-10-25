#pragma once
#include "input/interface/InputOption.h"
#include "deps.h"
// #include <string>
// #include <set>
// #include <functional>

namespace bashkir
{

class InputSpecialKey
{
private:
    const std::string key_seq;
    std::set<InputOption, std::greater<InputOption>> options;
public:
    InputSpecialKey(char spec_ch);
    InputSpecialKey(const std::string &csi_seq);
    InputSpecialKey(char spec_ch, const InputOption &opt);
    InputSpecialKey(const std::string &csi_seq, const InputOption &opt);
    InputSpecialKey(char spec_ch, const std::vector<InputOption> &opts);
    InputSpecialKey(const std::string &csi_seq, const std::vector<InputOption> &opts);
    void addOption(const InputOption &opt);

    bool operator==(const InputSpecialKey &key) const;
    bool operator!=(const InputSpecialKey &key) const;
};

} // namespace bashkir
