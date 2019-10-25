#include "input/InputSpecialKey.h"
#include "input/interface/CsiSequences.h"

namespace bashkir
{

InputSpecialKey::InputSpecialKey(char spec_ch)
    : key_seq(std::string(1, spec_ch)) {}

InputSpecialKey::InputSpecialKey(const std::string &csi_seq)
    : key_seq(csi_seq) {}

InputSpecialKey::InputSpecialKey(char spec_ch, const InputOption &opt)
    : key_seq(std::string(1, spec_ch)), options({ opt }) {}

InputSpecialKey::InputSpecialKey(const std::string &csi_seq, const InputOption &opt)
    : key_seq(csi_seq), options({ opt }) {}

InputSpecialKey::InputSpecialKey(char spec_ch, const std::vector<InputOption> &opts)
    : key_seq(std::string(1, spec_ch)), options(opt) {}

InputSpecialKey::InputSpecialKey(const std::string &csi_seq, const std::vector<InputOption> &opts)
    : key_seq(csi_seq), options(opt) {}

bool InputSpecialKey::operator==(const InputSpecialKey &key) const
{
    if (this->key_seq != key.key_seq)
    {
        return false;
    }
    return this->options == key.options;
}

bool InputSpecialKey::operator!=(const InputSpecialKey &key) const
{
    if (this->key_seq != key.key_seq)
    {
        return true;
    }
    return this->options != key.options;
}

} // namespace bashkir
