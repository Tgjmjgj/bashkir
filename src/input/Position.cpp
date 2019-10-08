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

bool Pos::operator>(const Pos &pos2) const
{
    return (this->line > pos2.line || (this->line == pos2.line && this->pos > pos2.pos));
}

bool Pos::operator<(const Pos &pos2) const
{
    return (this->line < pos2.line || (this->line == pos2.line && this->pos < pos2.pos));
}

bool Pos::operator>=(const Pos &pos2) const
{
    return (this->line > pos2.line || (this->line == pos2.line && this->pos >= pos2.pos));
}

bool Pos::operator<=(const Pos &pos2) const
{
    return (this->line < pos2.line || (this->line == pos2.line && this->pos <= pos2.pos));
}

} // namespace bashkir
