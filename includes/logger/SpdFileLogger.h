#pragma once
#include "deps.h"
// #include <memory>
// #include "spdlog/sinks/rotating_file_sink.h"
#include "logger/interface/BaseLogger.h"

namespace bashkir
{

const std::string DEFAULT_LOG_FILE_NAME = "logfile.log";
const uint DEFAULT_LOG_FILE_SIZE = 1048576 * 5; /// 5Mb
const uint8_t DEFAULT_LOG_FILE_NUMBER = 3;

class SpdFileLogger : public BaseLogger
{
private:
    std::shared_ptr<spdlog::logger> log;
public:
    SpdFileLogger(std::string filename = DEFAULT_LOG_FILE_NAME,
                  uint filesize = DEFAULT_LOG_FILE_SIZE,
                  uint8_t file_num = DEFAULT_LOG_FILE_NUMBER);
    ~SpdFileLogger();
    void Info(const std::string &msg);
    void Info(char ch);
    void Warn(const std::string &msg);
    void Err(const std::string &msg);

    void Flush();
};

} // namespace bashkir
