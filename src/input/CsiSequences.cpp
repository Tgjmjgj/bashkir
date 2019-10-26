#include "input/CsiSequences.h"

namespace bashkir::csi
{

CsiMatchStatus matchCsi(const std::string &str)
{
    for (const std::regex &csi_part : csi_parts)
    {
        if (std::regex_match(str, csi_part))
        {
            return CsiMatchStatus::PARTIAL_MATCH;
        }
    }
    if (std::regex_match(str, full_csi))
    {
        return CsiMatchStatus::FULL_MATCH;
    }
    return CsiMatchStatus::NOT_MATCH;
}

} // namespace bashkir::csi
