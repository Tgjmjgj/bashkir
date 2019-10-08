#pragma once
#include "deps.h"
// #include <string>
// #include <vector>
// #include <stack>
// #include <optional>
#include "input/Position.h"

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
    bool inline_exec;
    NestingRules rules;
public:
    BlockInfo& operator=(const BlockInfo &bi);
    bool operator==(const BlockInfo &bi) const;
    bool operator!=(const BlockInfo &bi) const;
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

class BlockPosData
{
public:
    const BlockInfo block; 
    Pos start_pos;
    const size_t uid;
public:
    BlockPosData(const BlockInfo &bl, const Pos &ps, size_t id);
};

class OpenBlock
{
public:
    const BlockInfo block;
    Pos start_pos;
    const bool escaped;
    const size_t uid;
private:
    static size_t last_uid;
public:
    OpenBlock(const BlockInfo &block, const Pos &pos, bool esc = false);
private:
    size_t getUid() const;
    static size_t getNewUid();
};

class AllBlocksData
{
public:
    std::stack<OpenBlock> open;
private:
    std::vector<BlockPosData> all;
public:
    void addOpen(Pos start_pos, const OpenBlock &bl);
    void addClose(Pos start_pos, const OpenBlock &bl);
    std::vector<BlockPosData> getFullList() const;
    void eraseAfterPos(const Pos &pos);
    void shiftAllAfterPos(const Pos &pos, int val);
};

} // namespace bashkir
