#include <iostream>
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

void StreamIO::writeStr(const std::string &str) const
{
    std::cout << str << '\n';
}

// Not implemented
void StreamIO::format(const std::string &fmt_str, ...) const
{
    std::cout << fmt_str;
}

// Not implemented
void StreamIO::formatStr(const std::string &fmt_str, ...) const
{
    std::cout << fmt_str << '\n';
}

void StreamIO::error(const std::string &err_msg) const
{
    std::cerr << err_msg;
}

std::string StreamIO::readStr() const
{
    std::string buf;
    std::getline(std::cin, buf);
    return buf;
}

char StreamIO::readChar() const
{
    return  static_cast<char>(std::getchar());
}

} // namespace bashkir
