#pragma once
#include "deps.h"
// #include <string>

namespace bashkir
{

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

class Pos
{
public:
    size_t line;
    size_t pos;

    Pos() = default;
    Pos(size_t new_line, size_t new_pos);
};

} // namespace bashkir
