#pragma once
#include "deps.h"
// #include <memory>
#include "logger/interface/BaseLogger.h"

namespace bashkir
{

class StdioLogger : public BaseLogger
{
public:
    StdioLogger();
    ~StdioLogger();
    void Info(const std::string &msg);
    void Info(char ch);
    void Warn(const std::string &msg);
    void Err(const std::string &msg);

    void Flush();
};

} // namespace bashkir
