#pragma once
#include <memory>
#include "logger/interface/BaseLogger.h"
#include "spdlog/sinks/rotating_file_sink.h"

namespace bashkir
{

#define LOG_FILE_NAME "logfile.log"
// 5mb
#define LOG_FILE_SIZE 1048576 * 5
#define LOG_FILE_NUMBER 3

class SpdFileLogger : public BaseLogger
{
private:
    std::shared_ptr<spdlog::logger> log;
public:
    SpdFileLogger();
    ~SpdFileLogger();
    inline void Info(const std::string &msg);
    inline void Info(char ch);
    inline void Warn(const std::string &msg);
    inline void Err(const std::string &msg);
};

} // namespace bashkir
