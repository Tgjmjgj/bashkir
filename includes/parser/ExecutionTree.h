#pragma once
#include "deps.h"
// #include <string>
// #include <memory>

namespace bashkir 
{

class ParseUnit
{
public:
    std::shared_ptr<ParseUnit> prev;
    std::shared_ptr<ParseUnit> next;
    std::string value;
    uint8_t level;
public:
    ParseUnit() = default;
    ParseUnit(ParseUnit&) = default;
    bool operator==(const ParseUnit &pu) const;
    bool operator!=(const ParseUnit &pu) const;
};

class ExecutionTree
{
private:
    std::shared_ptr<ParseUnit> start_point;

public:
    bool buildTree(const std::string &input);
    bool isCompletedCommand() const;
    std::shared_ptr<ParseUnit> getNextUnit() const;
    bool setInnerCommandResult(std::shared_ptr<ParseUnit> &target, const std::string &result);
};

} // namespace bashkir
