#include "logger/StdioLogger.h"
#include "global.h"

namespace bashkir
{

StdioLogger::StdioLogger() {}

StdioLogger::~StdioLogger() {}

void StdioLogger::Info(const std::string &msg)
{
    io.writeStr(msg);
}

void StdioLogger::Info(char ch)
{
    io.writeStr(ch + '\n');
}

void StdioLogger::Warn(const std::string &msg)
{
    io.writeStr(msg);
}

void StdioLogger::Err(const std::string &msg)
{
    io.error(msg);
}

void StdioLogger::Flush()
{
    return;
}

} // namespace bashkir
