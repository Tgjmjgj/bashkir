#include "input/BlockConstructions.h"
#include "global.h"
#include "util/strutil.h"
#include "util/stlutil.h"

namespace bashkir
{

BlockInfo& BlockInfo::operator=(const BlockInfo &bi)
{
    this->start_seq = bi.start_seq;
    this->end_seq = bi.end_seq;
    this->inline_exec = bi.inline_exec;
    this->rules = bi.rules;
    return *this;
}

bool BlockInfo::operator==(const BlockInfo &bi) const
{
    return this->start_seq == bi.start_seq && this->end_seq == bi.end_seq && this->inline_exec == bi.inline_exec;
}

bool BlockInfo::operator!=(const BlockInfo &bi) const
{
    return this->start_seq != bi.start_seq || this->end_seq != bi.end_seq || this->inline_exec != bi.inline_exec;
}

size_t OpenBlock::last_uid = 0;

OpenBlock::OpenBlock(const BlockInfo &bl, const Pos &pos, bool esc)
    : block(bl), start_pos(pos), escaped(esc), uid(OpenBlock::getNewUid()) {}

size_t OpenBlock::getNewUid()
{
    return OpenBlock::last_uid++;
}

Blocks::Blocks()
{
    // { start_seq, end_seq, inline_exec, rules={ all, esc, allowed } }
    const BlockInfo b1 = { "$(", ")", false, { true, false, {} } };
    const BlockInfo b2 = { "[", "]", false, { true, false, {} } };
    const BlockInfo b3 = { "{", "}", false, { true, false, {} } };
    const BlockInfo b4 = { "'", "'", false, { false, false, {} } };
    BlockInfo b5 = { "`", "`", true, { false, true, { /*b5*/ } } };
    b5.rules.allowed.push_back(b5);
    const BlockInfo b6 = { "\"", "\"", false, { false, false, { b1, b5 } } };
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

BlockPosData::BlockPosData(const BlockInfo &bl, const Pos &spos, size_t id)
    : block(bl), start_pos(spos), uid(id) {}

void AllBlocksData::addOpen(Pos start_pos, const OpenBlock &bl)
{
    this->all.push_back(BlockPosData(bl.block, start_pos, bl.uid));
}

void AllBlocksData::addClose(Pos start_pos, const OpenBlock &bl)
{
    assert(util::filter(
                this->all,
                [this, &bl](const BlockPosData &blpos){ return blpos.uid == bl.uid; }
            ).size() == 1);
    this->all.push_back(BlockPosData(bl.block, start_pos, bl.uid));
}

std::vector<BlockPosData> AllBlocksData::getFullList() const
{
    return this->all;
}

void AllBlocksData::eraseAfterPos(const Pos &pos)
{
    if (!this->open.empty())
    {
        Pos &top = this->open.top().start_pos;
        while (top >= pos && !this->open.empty())
        {
            top = this->open.top().start_pos;
            this->open.pop();
        }
    }
    if (!this->all.empty())
    {
        Pos &top = this->all.back().start_pos;
        while (top >= pos && !this->all.empty())
        {
            top = this->all.back().start_pos;
            this->all.pop_back();
        }
    }
}

void AllBlocksData::shiftAllAfterPos(const Pos &pos, int val)
{
    throw std::logic_error("Not implemented yet!");
}

} // namespace bashkir
