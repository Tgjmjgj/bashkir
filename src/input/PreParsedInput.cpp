// #include <algorithm>
#include "input/PreParsedInput.h"
#include "input/Position.h"
#include "util/stlutil.h"
#include "util/strutil.h"

namespace bashkir
{

CompressedBlockData::CompressedBlockData(const BlockInfo &bl, size_t pos, bool start, size_t id)
    : block(bl), start_pos(pos), is_start(start), uid(id) {}

PreParsedInput::PreParsedInput(const std::vector<CompressedBlockData> &cbds, const std::string &input)
    : blocks(cbds), str(input) {}

size_t compressPos(const Pos &pos, const std::vector<Line> &lines)
{
    size_t cpos = pos.pos;
    for (int i = 0; i < static_cast<int>(pos.line) - 1; ++i)
    {
        cpos += lines[i].real_length;
    }
    return cpos;
}

std::string compressInput(const std::vector<Line> &lines)
{
    return util::join(lines, "\n");
}

PreParsedInput compressInputData(const std::vector<BlockPosData> &bpds, const std::vector<Line> &lines)
{
    std::vector<size_t> ids;
    const std::vector<CompressedBlockData> blocks_in_input =
        util::map<BlockPosData, CompressedBlockData>(
            bpds,
            [&lines, &ids](const BlockPosData &bpd) {
                size_t new_pos = compressPos(bpd.start_pos, lines);
                bool is_start = true;
                if (auto found = std::find(ids.begin(), ids.end(), bpd.uid); found == ids.end())
                {
                    ids.push_back(bpd.uid);
                }
                else
                {
                    ids.erase(found);
                    is_start = false;
                }
                return CompressedBlockData(bpd.block, new_pos, is_start, bpd.uid);
            }
        );
    const std::string combined_input_str = compressInput(lines);
    return PreParsedInput(blocks_in_input, combined_input_str);
}

} // namespace bashkir
