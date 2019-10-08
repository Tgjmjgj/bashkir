#pragma once
#include "deps.h"
// #include <memory>
// #include <vector>
// #include <string>
// #include <optional>
#include "input/Position.h"
#include "input/BlockConstructions.h"
#include "input/PreParsedInput.h"

namespace bashkir
{

class InputHandler
{
private:
    char tmp_buf[input_buffer_size];
    
    const std::shared_ptr<std::vector<std::string>> hist;

    std::vector<Line> input;
    Pos cur;
    std::size_t mem;

    std::size_t hist_ind;

    AllBlocksData blocks;

    bool end = false;

public:
    InputHandler(std::shared_ptr<std::vector<std::string>> history);
    PreParsedInput waitInput();
private:
    void writePrefix();
    void runInputLoop();

    void rebuildBlocksData(const Pos &from_pos);
    void detectBlocks(const Pos &inpos);

    void setPos(size_t p) noexcept;
    bool isPosEscaped(size_t pos) const;
    bool isPosEscaped(size_t pos, size_t line) const;
    bool isCurPosEscaped() const;

    std::optional<std::string> lookForCSISequenceInPos(size_t pos);
    void pressCSIsequence(std::string csi_seq);
    void pressSimpleKey(char ch);
    void writeChars(const std::string &chars);
    void setHistoryItem();
    void addNewInputLine();
    bool removeInputLine();

    bool moveCursorLeft();
    bool moveCursorRight();
    bool moveCursorUp();
    bool moveCursorDown();
    void moveCursorVertically(size_t from, size_t to);
    bool removeFromLeft();
    bool removeFromRight();
};

} // namespace bashkir
