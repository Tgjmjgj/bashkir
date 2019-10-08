#pragma once
#include "deps.h"
// #include <string>
// #include <vector>
#include "input/BlockConstructions.h"

namespace bashkir
{

class CompressedBlockData
{
public:
    const BlockInfo block;
    const size_t start_pos;
    const bool is_start;
    const size_t uid;
public:
    CompressedBlockData(const BlockInfo &bl, size_t pos, bool start, size_t id);
};

class PreParsedInput
{
public:
    const std::vector<CompressedBlockData> blocks;
    const std::string str;
public:
    PreParsedInput(const std::vector<CompressedBlockData> &cbds, const std::string &input);
};

PreParsedInput compressInputData(const std::vector<BlockPosData> &bpds, const std::vector<Line> &lines);

} // namespace bashkir
