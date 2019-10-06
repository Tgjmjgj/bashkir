#pragma once
#include "deps.h"
// #include <string>
// #include <vector>
// #include <optional>
#include "global.h"

namespace bashkir
{

class BlockInfo;

class NestingRules
{
public:
    bool all;
    bool esc;
    std::vector<BlockInfo> allowed;
};

class BlockInfo
{
public:
    std::string start_seq;
    std::string end_seq;
    NestingRules rules;

    bool operator==(const BlockInfo &bi) const;
    bool operator!=(const BlockInfo &bi) const;
};

class OpenBlock
{
public:
    const BlockInfo block;
    const bool escaped;
    OpenBlock(const BlockInfo &block, bool esc = false);
};

class Blocks
{
    using iterator = std::vector<BlockInfo>::iterator;
    using const_iterator = std::vector<BlockInfo>::const_iterator;
    using value_type = BlockInfo;
public:
    std::vector<BlockInfo> blocks;
public:
    Blocks();
    std::optional<BlockInfo> findByStart(std::string by_end) const;
    std::optional<BlockInfo> findByEnd(std::string by_start) const;
    std::optional<BlockInfo> searchStartBeforePos(const char *str, size_t pos) const;
    std::optional<BlockInfo> searchEndBeforePos(const char *str, size_t pos) const;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
};

} // namespace bashkir
