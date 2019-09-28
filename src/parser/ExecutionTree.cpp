#include "assert.h"
#include "parser/ExecutionTree.h"

namespace bashkir
{

const std::string inner_cmd_start = "$(";
const std::string inner_cmd_end = ")";

bool ExecutionTree::buildTree(const std::string &input)
{
    if (input.length() == 0)
    {
        return false;
    }
    std::shared_ptr<ParseUnit> pu = std::make_shared<ParseUnit>();
    pu->level = 0;
    pu->prev = nullptr;
    this->start_point = pu;
    size_t st_ind = 0, en_ind = 0;
    size_t last_separation_ind = 0;
    for (size_t i = 0; i < input.length(); ++i)
    {
        if (input[i] == inner_cmd_start[st_ind])
        {
            if (st_ind == inner_cmd_start.length() - 1)
            {
                pu->value = input.substr(
                    last_separation_ind,
                    i - last_separation_ind - (inner_cmd_start.length() - 1)
                );
                last_separation_ind = i + 1;
                std::shared_ptr<ParseUnit> new_pu = std::make_shared<ParseUnit>();
                new_pu->level = pu->level + 1;
                new_pu->prev = pu;
                pu->next = new_pu;
                pu = new_pu;
            }
            else
            {
                ++st_ind;
            }
        }
        if (input[i] == inner_cmd_end[en_ind])
        {
            if (en_ind == inner_cmd_end.length() - 1)
            {
                pu->value = input.substr(
                    last_separation_ind,
                    i - last_separation_ind - (inner_cmd_end.length() - 1)
                );
                last_separation_ind = i + 1;
                std::shared_ptr<ParseUnit> new_pu = std::make_shared<ParseUnit>();
                new_pu->level = pu->level - 1;
                new_pu->prev = pu;
                pu->next = new_pu;
                pu = new_pu;

            }
            else
            {
                ++en_ind;
            }
        }
    }
    pu->value = input.substr(last_separation_ind);
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

bool ParseUnit::isFinal() const
{
    return this->level == 0 && this->prev == nullptr && this->next == nullptr;
}

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

ParseUnitIterator ExecutionTree::begin()
{
    return ParseUnitIterator(this->start_point);
}

const ParseUnitIterator ExecutionTree::begin() const
{
    return ParseUnitIterator(this->start_point);
}

ParseUnitIterator ExecutionTree::end()
{
    return ParseUnitIterator(nullptr);
}

const ParseUnitIterator ExecutionTree::end() const
{
    return ParseUnitIterator(nullptr);
}

std::shared_ptr<ParseUnit> ParseUnitIterator::getCurrentParseUnit() const
{
    std::shared_ptr<ParseUnit> pu = this->start_point;
    while (pu != nullptr && pu->next != nullptr && pu->next->level > pu->level)
    {
        pu = pu->next;
    }
    return pu;
}

ParseUnitIterator::ParseUnitIterator(std::shared_ptr<ParseUnit> start_p) : start_point(std::move(start_p))
{
    this->current_pu = this->getCurrentParseUnit();
    this->pu_copy = this->current_pu == nullptr ? nullptr : std::make_unique<ParseUnit>(*(this->current_pu));
}

std::shared_ptr<ParseUnit> ParseUnitIterator::operator*() const
{
    return this->current_pu;
}

void ParseUnitIterator::operator++()
{
    this->current_pu = this->getCurrentParseUnit();
    if (this->current_pu != nullptr && *(this->current_pu) == *(this->pu_copy))
    {
        this->current_pu = nullptr;
    }
}

bool ParseUnitIterator::operator==(const ParseUnitIterator &it) const
{
    return this->current_pu == it.current_pu;
}

bool ParseUnitIterator::operator!=(const ParseUnitIterator &it) const
{
    return this->current_pu != it.current_pu;
}

} // namespace bashkir
