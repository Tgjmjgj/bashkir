#pragma once
#include "deps.h"
// #include <memory>
// #include <vector>
// #include <string>
// #include <optional>
#include "input/Position.h"
#include "input/BlockConstructions.h"
#include "input/PreParsedInput.h"
#include "input/interface/IAutocomplete.h"
#include "input/KeyBindingsMap.h"

namespace bashkir
{

class InputActions;

class InputHandler
{
    friend class InputActions;
private:

    enum class Mode : uint16_t
    {
        SINGLELINE,
        MULTILINE
    };

    KeyBindingsMap keymap;
    char tmp_buf[input_buffer_size];
    
    const std::shared_ptr<std::vector<std::string>> hist;

    std::vector<Line> input;
    Pos cur;
    std::size_t mem;

    std::size_t hist_ind;

    std::unique_ptr<IAutocomplete> autoc;
    AllBlocksData blocks;

    Mode mode = Mode::SINGLELINE;
    bool end = false;
    bool untouched = false;

public:
    InputHandler(std::shared_ptr<std::vector<std::string>> history);
    PreParsedInput waitInput();
private:
    void clearInput();
    void flushState();
    void writePrefix();
    void runInputLoop();

    void rebuildBlocksData(const Pos &from_pos);
    void detectBlocks(const Pos &inpos);

    void setPos(size_t p) noexcept;
    bool isPosEscaped(size_t pos) const;
    bool isPosEscaped(size_t pos, size_t line) const;
    bool isCurPosEscaped() const;

    void pressCsiSequence(const std::string &csi_seq);
    void pressSimpleKey(char ch);
    void writeChars(const std::string &chars);

};

} // namespace bashkir
