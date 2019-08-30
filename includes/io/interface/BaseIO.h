#pragma once
#include <string>

namespace bashkir
{

class BaseIO
{
public:
    virtual ~BaseIO() = default;
    virtual void write(const std::string &str) const = 0;
    virtual void writeStr(const std::string &str) const = 0;
    virtual void formatStr(const std::string &str, ...) const = 0;
    virtual void error(const std::string &err_msg) const = 0;
    virtual std::string readStr() const = 0;
    virtual char readChar() const = 0;
};

} // namespace bashkir
