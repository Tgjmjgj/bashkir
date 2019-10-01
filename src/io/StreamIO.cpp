#include "deps.h"
// #include <iostream>
// #include <stdarg.h>
#include "io/StreamIO.h"

namespace bashkir
{

StreamIO::StreamIO() {}

void StreamIO::write(const std::string &str) const
{
    std::cout << str;
}

void StreamIO::write(const char ch) const
{
    std::cout << ch;
}

void StreamIO::write(int num) const
{
    std::cout << num;
}

void StreamIO::writeStr(const std::string &str) const
{
    std::cout << str << '\n';
}

void StreamIO::writeStr(int num) const
{
    std::cout << num << '\n';
}

void StreamIO::format(const std::string &fmt_str, ...) const
{
    va_list arg_ptr;
    va_start(arg_ptr, fmt_str);
    vprintf(fmt_str.c_str(), arg_ptr);
    va_end(arg_ptr);
}

void StreamIO::formatStr(const std::string &fmt_str, ...) const
{
    va_list arg_ptr;
    va_start(arg_ptr, fmt_str);
    vprintf(fmt_str.c_str(), arg_ptr);
    va_end(arg_ptr);
    printf("\n");
}

void StreamIO::error(const std::string &err_msg) const
{
    std::cerr << err_msg << '\n';
}

std::string StreamIO::readStr() const
{
    std::string buf;
    std::getline(std::cin, buf);
    return buf;
}

char StreamIO::readChar() const
{
    return static_cast<char>(std::getchar());
}

void StreamIO::flush() const
{
    std::cout.flush();
}

} // namespace bashkir
