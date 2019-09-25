#pragma once
#include "deps.h"
// #include <string>
// #include <memory>
#include "io/interface/BaseIO.h"

namespace bashkir
{

class StreamIO : public BaseIO
{
public:
    StreamIO();
    ~StreamIO() {};
    void write(const std::string &str) const;
    void write(const char ch) const;
    void writeStr(const std::string &str) const;
    void format(const std::string &fmt_str, ...) const;
    void formatStr(const std::string &fmt_str, ...) const;
    void error(const std::string &err_msg) const;
    std::string readStr() const;
    char readChar() const;
};

} // namespace bashkir
