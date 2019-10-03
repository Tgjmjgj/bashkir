#include "logger/SpdFileLogger.h"

#include "spdlog/async.h"

namespace bashkir
{

SpdFileLogger::SpdFileLogger(std::string filename, uint filesize, uint8_t file_num)
{
    this->log = spdlog::rotating_logger_mt("spd", filename, filesize, file_num);
    // spdlog::flush_every(std::chrono::seconds(5));
}

SpdFileLogger::~SpdFileLogger() {}

void SpdFileLogger::Info(const std::string &msg)
{
    this->log->info(msg);
}

void SpdFileLogger::Info(char ch)
{
    this->log->info(ch);
}

void SpdFileLogger::Warn(const std::string &msg)
{
    this->log->warn(msg);
}

void SpdFileLogger::Err(const std::string &msg)
{
    this->log->error(msg);
}

void SpdFileLogger::Flush()
{
    this->log->flush();
}

} // namespace bashkir
