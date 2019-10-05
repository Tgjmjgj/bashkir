#pragma once
#include "deps.h"
// #include <memory>
// #include <vector>
// #include <string>
// #include <optional>

namespace bashkir
{

class Pos
{
public:
    size_t line;
    size_t pos;
    size_t mem_pos;

    void Set(size_t new_pos) noexcept;
};

inline const size_t max_line_length = 2048;
inline const size_t input_buffer_size = 16;
inline const std::string new_line_prefix = "> ";

class Line
{
public:
    char data[max_line_length];
    size_t real_length = 0;
    std::string prefix = new_line_prefix;

    Line();
    Line(const std::string &init);
};

class InputHandler
{
private:
    char tmp_buf[input_buffer_size];
    
    const std::shared_ptr<std::vector<std::string>> hist;

    std::vector<Line> input;
    Pos cur_pos;
    std::size_t hist_ind;
    std::stack<std::string> opened_blocks;
    bool escaped_next = false;
    bool end = false;

public:
    InputHandler(std::shared_ptr<std::vector<std::string>> history);
    std::string waitInput();
private:
    void writePrefix();
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
