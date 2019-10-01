// #include <stdarg.h>
#include "io/NcursesIO.h"
#include "wrappers/NCurses.h"

namespace bashkir
{

NcursesIO::NcursesIO(std::shared_ptr<NCurses> ncurses)
    : nc(std::move(ncurses)) {}

void NcursesIO::write(const std::string &str) const
{
    this->nc->write(str);
}

void NcursesIO::write(const char ch) const
{
    this->nc->writeChar(ch);
}

void NcursesIO::write(int num) const
{
    this->nc->write("stub");
}

void NcursesIO::writeStr(const std::string &str) const
{
    this->nc->writeStr(str);
}

void NcursesIO::writeStr(int num) const
{
    this->nc->writeStr("stub");
}

void NcursesIO::format(const std::string &fmt_str, ...) const
{
    va_list arg_ptr;
    va_start(arg_ptr, fmt_str);
    this->nc->format(fmt_str, arg_ptr);
    va_end(arg_ptr);
}

void NcursesIO::formatStr(const std::string &fmt_str, ...) const
{
    va_list arg_ptr;
    va_start(arg_ptr, fmt_str);
    this->nc->formatStr(fmt_str, arg_ptr);
    va_end(arg_ptr);
}

void NcursesIO::error(const std::string &err_msg) const
{
    this->nc->writeError(err_msg);
}

std::string NcursesIO::readStr() const
{
    return this->nc->readStr();
}

char NcursesIO::readChar() const
{
    return 'S';
}

void NcursesIO::flush() const
{
    return;
}

} // namespace bashkir
