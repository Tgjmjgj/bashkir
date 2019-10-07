#include "input/Position.h"

namespace bashkir
{

Pos::Pos(size_t new_line, size_t new_pos)
    : line(new_line), pos(new_pos) {}

Line::Line()
{
    memset(this->data, 0, max_line_length);
}

Line::Line(const std::string &init) : Line()
{
    if (init.length() >= max_line_length)
    {
        throw std::length_error("Max input line length exceeded");
    }
    strcpy(this->data, init.c_str());
    this->real_length = init.length();
}

} // namespace bashkir
