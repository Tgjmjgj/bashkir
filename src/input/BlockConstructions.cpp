#include "input/BlockConstructions.h"
#include "util/strutil.h"
#include "util/stlutil.h"

namespace bashkir
{

BlockInfo& BlockInfo::operator=(const BlockInfo &bi)
{
    this->start_seq = bi.start_seq;
    this->end_seq = bi.end_seq;
    this->rules = bi.rules;
    return *this;
}

bool BlockInfo::operator==(const BlockInfo &bi) const
{
    return this->start_seq == bi.start_seq && this->end_seq == bi.end_seq;
}

bool BlockInfo::operator!=(const BlockInfo &bi) const
{
    return this->start_seq != bi.start_seq || this->end_seq != bi.end_seq;
}

size_t OpenBlock::last_uid = 0;

OpenBlock::OpenBlock(const BlockInfo &bl, bool esc)
    : block(bl), escaped(esc), uid(OpenBlock::getNewUid()) {}

size_t OpenBlock::getNewUid()
{
    return OpenBlock::last_uid++;
}

Blocks::Blocks()
{
    // { start_seq, end_seq, rules={ all, esc, allowed } }
    const BlockInfo b1 = { "$(", ")", { true, false, {} } };
    const BlockInfo b2 = { "[", "]", { true, false, {} } };
    const BlockInfo b3 = { "{", "}", { true, false, {} } };
    const BlockInfo b4 = { "'", "'", { false, false, {} } };
    BlockInfo b5 = { "`", "`", { false, true, { /*b5*/ } } };
    b5.rules.allowed.push_back(b5);
    const BlockInfo b6 = { "\"", "\"", { false, false, { b1, b5 } } };
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

BlockPosData::BlockPosData()
    : start_pos({ 0, 0 }), seq(""), uid(0) {} 

BlockPosData::BlockPosData(Pos spos, const std::string &sq, size_t id)
    : start_pos(spos), seq(sq), uid(id) {}

void AllBlocksData::addOpen(Pos start_pos, const OpenBlock &bl)
{
    this->all.push_back(BlockPosData(start_pos, bl.block.start_seq, bl.uid));
}

void AllBlocksData::addClose(Pos start_pos, const OpenBlock &bl)
{
    assert(util::filter(
                this->all,
                [this, &bl](const BlockPosData &blpos){ return blpos.uid == bl.uid; }
            ).size() == 1);
    this->all.push_back(BlockPosData(start_pos, bl.block.end_seq, bl.uid));
}

} // namespace bashkir
