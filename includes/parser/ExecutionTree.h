#pragma once
#include "deps.h"
// #include <string>
// #include <memory>
#include "input/PreParsedInput.h"

namespace bashkir 
{

class ParseUnit
{
public:
    uint8_t level;
    std::shared_ptr<ParseUnit> prev;
    std::shared_ptr<ParseUnit> next;
    std::string value;
public:
    ParseUnit() = default;
    ParseUnit(ParseUnit&) = default;
    ParseUnit(uint8_t lev, std::shared_ptr<ParseUnit> prv, std::shared_ptr<ParseUnit> nxt);
    bool operator==(const ParseUnit &pu) const;
    bool operator!=(const ParseUnit &pu) const;
};

class ExecutionTree
{
private:
    std::shared_ptr<ParseUnit> start_point;

public:
    bool buildTree(const PreParsedInput &input);
    bool isCompletedCommand() const;
    std::shared_ptr<ParseUnit> getNextUnit() const;
    bool setInnerCommandResult(std::shared_ptr<ParseUnit> &target, const std::string &result);
};

} // namespace bashkir
