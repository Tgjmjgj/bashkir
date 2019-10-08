// #include <assert.h>
// #include <algorithm>
#include "parser/ExecutionTree.h"
#include "global.h"
#include "util/stlutil.h"

namespace bashkir
{

const std::vector<BlockInfo> inline_blocks = 
    util::filter(global::blocksMeta.blocks, [](const BlockInfo &bi) { return bi.inline_exec; });

bool ExecutionTree::buildTree(const PreParsedInput &input)
{
    std::shared_ptr<ParseUnit> pu = std::make_shared<ParseUnit>();
    this->start_point = pu;
    size_t last_separation_ind = 0;
    for (const CompressedBlockData &block : input.blocks)
    {
        if (std::find(inline_blocks.begin(), inline_blocks.end(), block.block) != inline_blocks.end())
        {
            const size_t seq_len = (block.is_start ? block.block.start_seq : block.block.end_seq).length();
            pu->value = input.str.substr(last_separation_ind, block.start_pos - last_separation_ind);
            last_separation_ind = block.start_pos + seq_len;
            const size_t new_lvl = pu->level + (block.is_start ? 1 : - 1);
            std::shared_ptr<ParseUnit> new_pu = std::make_shared<ParseUnit>(new_lvl, pu, nullptr);
            pu->next = new_pu;
            pu = new_pu;
        }
    }
    pu->value = input.str.substr(last_separation_ind);
    pu->next = nullptr;
    return true;
}

bool ExecutionTree::isCompletedCommand() const
{
    return this->start_point->prev == nullptr && this->start_point->next == nullptr;
}

std::shared_ptr<ParseUnit> ExecutionTree::getNextUnit() const
{
    std::shared_ptr<ParseUnit> pu = this->start_point;
    while (pu != nullptr && pu->next != nullptr && pu->level < pu->next->level)
    {
        pu = pu->next;
    }
    return pu;
}

bool ExecutionTree::setInnerCommandResult(std::shared_ptr<ParseUnit> &target, const std::string &result)
{
    if (target->prev == nullptr && target->next == nullptr)
    {
        return false;
    }
    std::shared_ptr<ParseUnit> newParseUnit = std::make_shared<ParseUnit>();
    newParseUnit->value = target->prev->value + result + target->next->value;
    newParseUnit->prev = target->prev->prev;
    newParseUnit->next = target->next->next;
    assert((target->prev->level == target->next->level));
    newParseUnit->level = target->prev->level;
    if (target->prev->prev != nullptr)
    {
        target->prev->prev->next = newParseUnit;
    }
    else 
    {
        this->start_point = newParseUnit;
    }
    if (target->next->next != nullptr)
    {
        target->next->next->prev = newParseUnit;
    }
    target->prev = target->next = nullptr;
    return true;
}

ParseUnit::ParseUnit(uint8_t lev, std::shared_ptr<ParseUnit> prv, std::shared_ptr<ParseUnit> nxt)
    : level(lev), prev(prv), next(nxt) {}

bool ParseUnit::operator==(const ParseUnit &pu) const
{
    return (
        this->level == pu.level &&
        this->value == pu.value &&
        this->prev.get() == pu.prev.get() &&
        this->next.get() == pu.next.get()
    );
}

bool ParseUnit::operator!=(const ParseUnit &pu) const
{
    return !(*this == pu);
}

} // namespace bashkir
