#pragma once
#include "deps.h"
// #include <memory>
// #include <vector>
// #include <string>

namespace bashkir
{

struct Pos
{
    size_t line;
    size_t pos;
};

inline const size_t max_line_length = 2048;
inline const size_t input_buffer_size = 10;

struct Line
{
    char data[max_line_length];
    size_t real_length = 0;
    Line();
    Line(const std::string &init);
};

class InputHandler
{
private:
    char tmp_buf[input_buffer_size];
    
    const std::shared_ptr<std::vector<std::string>> hist;

    std::string first_line_prefix;

    std::vector<Line> input;
    Pos cur_pos;
    std::size_t hist_ind;
    std::stack<std::string> opened_blocks;
    bool escaped_next = false;
    bool end = false;

public:
    InputHandler(std::shared_ptr<std::vector<std::string>> history);
    void writePrefix();
    std::string waitInput();
private:
    void pressCSIsequence(std::string csi_seq);
    void pressSimpleKey(char ch);
    void writeChars(const std::string &chars);
    void setHistoryItem();
    void addNewInputLine();
    void removeInputLine();
};

} // namespace bashkir
