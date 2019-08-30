#pragma once
#include <string>
#include <cstdarg>

namespace bashkir
{

const int MAX_INPUT_SIZE = 1024;

struct NCoord
{
    int x;
    int y;
    NCoord(int x_val, int y_val)
        : x(x_val), y(y_val) {}
};

class NCurses
{
private:
    char *read_buffer;

public:
    NCurses();
    ~NCurses();
    NCoord getCursorPos() const;
    void setCursorPos(const NCoord &coord) const;
    void write(const std::string &str) const;
    void writeStr(const std::string &str) const;
    void writeChar(const char ch) const;
    void writeError(const std::string &err_msg) const;
    void formatStr(const std::string &fmt_str, ...) const;
    void formatStr(const std::string &fmt_str, va_list argptr) const;
    std::string readStr() const;
};

} // namespace bashkir
