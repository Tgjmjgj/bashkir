#pragma once

#include <string>
#include <memory>

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
    bool isFinal() const;
    bool operator==(const ParseUnit &pu) const;
    bool operator!=(const ParseUnit &pu) const;
};

class ParseUnitIterator
{
private:
    std::shared_ptr<ParseUnit> start_point;
    std::shared_ptr<ParseUnit> current_pu;
    std::unique_ptr<ParseUnit> pu_copy;
    bool end = false;
public:
    ParseUnitIterator(std::shared_ptr<ParseUnit> start_p);
    std::shared_ptr<ParseUnit> operator*() const;
    void operator++();
    bool operator ==(const ParseUnitIterator &it) const;
    bool operator !=(const ParseUnitIterator &it) const;
private:
    std::shared_ptr<ParseUnit> getCurrentParseUnit() const;
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

    ParseUnitIterator begin();
    const ParseUnitIterator begin() const;
    ParseUnitIterator end();
    const ParseUnitIterator end() const;
};

} // namespace bashkir
