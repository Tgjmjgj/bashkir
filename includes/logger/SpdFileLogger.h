#pragma once
#include "deps.h"
// #include <memory>
// #include "spdlog/sinks/rotating_file_sink.h"
#include "logger/interface/BaseLogger.h"

namespace bashkir
{

const char *const LOG_FILE_NAME = "logfile.log";
const uint LOG_FILE_SIZE = 1048576 * 5; /// 5Mb
const uint8_t LOG_FILE_NUMBER = 3;

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
