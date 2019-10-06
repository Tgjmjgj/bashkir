#include "input/BlockConstructions.h"
#include "util/strutil.h"

namespace bashkir
{

bool BlockInfo::operator==(const BlockInfo &bi) const
{
    return this->start_seq == bi.start_seq && this->end_seq == bi.end_seq;
}

bool BlockInfo::operator!=(const BlockInfo &bi) const
{
    return this->start_seq != bi.start_seq || this->end_seq != bi.end_seq;
}

OpenBlock::OpenBlock(const BlockInfo &bl, bool esc)
    : block(bl), escaped(esc) {}

Blocks::Blocks()
{
    // { start_seq, end_seq, rules={ all, esc, allowed } }
    const BlockInfo b1 = { "$(", ")", { true, false, {} } };
    const BlockInfo b2 = { "[", "]", { true, false, {} } };
    const BlockInfo b3 = { "{", "}", { true, false, {} } };
    const BlockInfo b4 = { "'", "'", { false, false, {} } };
    const BlockInfo b5 = { "`", "`", { false, false, {} } };
    const BlockInfo b6 = { "\"", "\"", { false, true, { b1, b5 } } };
    this->blocks = { b1, b2, b3, b4, b5, b6 };
}

std::optional<BlockInfo> Blocks::findByStart(std::string start) const
{
    for (auto& block : this->blocks)
    {
        if (block.start_seq == start)
        {
            return block;
        }
    }
    return std::nullopt;
}

std::optional<BlockInfo> Blocks::findByEnd(std::string end) const
{
    for (auto& block : this->blocks)
    {
        if (block.end_seq == end)
        {
            return block;
        }
    }
    return std::nullopt;
}

std::optional<BlockInfo> Blocks::searchStartBeforePos(const char *str, size_t pos) const
{
    if (pos > strlen(str))
    {
        return std::nullopt;
    }
    for (auto& block : this->blocks)
    {
        if (util::isSubstrBeforePos(str, block.start_seq.c_str(), pos))
        {
            return block;
        }
    }
    return std::nullopt;
}

std::optional<BlockInfo> Blocks::searchEndBeforePos(const char *str, size_t pos) const
{
    if (pos > strlen(str))
    {
        return std::nullopt;
    }
    for (auto& block : this->blocks)
    {
        if (util::isSubstrBeforePos(str, block.end_seq.c_str(), pos))
        {
            return block;
        }
    }
    return std::nullopt;
}

Blocks::iterator Blocks::begin()
{
    return this->blocks.begin();
}

Blocks::iterator Blocks::end()
{
    return this->blocks.end();
}

Blocks::const_iterator Blocks::begin() const
{
    return this->blocks.begin();
}

Blocks::const_iterator Blocks::end() const
{
    return this->blocks.end();
}

} // namespace bashkir
