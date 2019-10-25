#include "input/CsiSequences.h"

namespace bashkir::csi
{

CsiMatchStatus matchCsi(const std::string &str)
{
    if (std::regex_match(str, full_csi))
    {
        return CsiMatchStatus::FULL_MATCH;
    }
    if (std::regex_match(str, part_csi))
    {
        return CsiMatchStatus::PARTIAL_MATCH;
    }
    return CsiMatchStatus::NOT_MATCH;
}

CsiSequence::CsiSequence(const CommandStart &strt, const std::vector<uint> &prms, const std::vector<char> &interm, char trm)
    : start(strt), params(prms), mid(interm), term(trm) {}

const std::string CsiSequence::getStrWithoutModifiers() const
{
}

const CsiSequence convertToCsiObject(const std::string &csi_str)
{
    CommandStart start = CommandStart::CSI;
    std::vector<uint> params;
    std::vector<uint> interm;
    char term = 'd';
    return CsiSequence(start, params, interm, term);
}

inline InputSpecialKey extractOptions(const std::string &csi_seq)
{
    std::vector<InputOption> opts;

    // ALL WORK HERE
    const std::string simplified_csi = csi_seq;
    // CONTINUE

    return InputSpecialKey(simplified_csi, opts);
}

} // namespace bashkir::csi
