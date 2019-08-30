#include <string>
#include "wrappers/NCurses.h"
#include "ncurses/ncurses.h"
#include "util/convutil.h"

namespace bashkir
{

NCurses::NCurses()
{
    this->read_buffer = new char[MAX_INPUT_SIZE];
    initscr();
    cbreak();
    echo();
    keypad(stdscr, true);
}

NCurses::~NCurses()
{
    endwin();
}

NCoord NCurses::getCursorPos() const
{
    // getyx it is macro, so there is no errors with value-args
    int x, y;
    getyx(stdscr, x, y);
    return NCoord(x, y);
}

void NCurses::setCursorPos(const NCoord &coord) const
{
}

void NCurses::write(const std::string &str) const
{
    addstr(str.c_str());
    refresh();
}

void NCurses::writeStr(const std::string &str) const
{
    addstr(str.c_str());
    addch(util::c2ui('\n'));
    refresh();
}

void NCurses::writeChar(const char ch) const
{
    addch(util::c2ui(ch));
    refresh();
}

void NCurses::writeError(const std::string &err_msg) const
{
    addstr(err_msg.c_str());
    addch('\n');
    refresh();
}

void NCurses::formatStr(const std::string &fmt_str, ...) const
{
    va_list arg_ptr;
    const char *c_str = fmt_str.c_str();
    va_start(arg_ptr, fmt_str);
    vwprintw(stdscr, c_str, arg_ptr);
    va_end(arg_ptr);
}

void NCurses::formatStr(const std::string &fmt_str, va_list arg_ptr) const
{
    vwprintw(stdscr, fmt_str.c_str(), arg_ptr);
}

std::string NCurses::readStr() const
{
    getstr(this->read_buffer);
    return std::string(this->read_buffer);
}

} // namespace bashkir
