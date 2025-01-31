#pragma once
#include "deps.h"
// #include <string>

namespace bashkir
{

class BaseLogger
{
public:
    virtual ~BaseLogger() = default;
    virtual void Info(const std::string &msg) = 0;
    virtual void Info(char ch) = 0;
    virtual void Warn(const std::string &msg) = 0;
    virtual void Err(const std::string &msg) = 0;
};

} // namespace bashkir
