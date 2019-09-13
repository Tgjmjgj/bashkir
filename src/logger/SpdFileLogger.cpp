#include "logger/SpdFileLogger.h"

namespace bashkir
{

SpdFileLogger::SpdFileLogger()
{
    this->log = spdlog::rotating_logger_mt("spd", LOG_FILE_NAME, LOG_FILE_SIZE, LOG_FILE_NUMBER);
}

SpdFileLogger::~SpdFileLogger() {}

inline void SpdFileLogger::Info(const std::string &msg)
{
    this->log->info(msg);
}

inline void SpdFileLogger::Info(char ch)
{
    this->log->info(ch);
}

inline void SpdFileLogger::Warn(const std::string &msg)
{
    this->log->warn(msg);
}

inline void SpdFileLogger::Err(const std::string &msg)
{
    this->log->error(msg);
}

} // namespace bashkir
