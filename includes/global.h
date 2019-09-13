#pragma once
#include <memory>
#include "logger/interface/BaseLogger.h"
#include "logger/SpdFileLogger.h"

namespace bashkir
{

#define LOG_L1 log::level > 0
#define LOG_L2 log::level > 1
#define LOG_L3 log::level > 2

namespace log
{

inline int level = 2;

inline std::unique_ptr<BaseLogger> to = std::make_unique<SpdFileLogger>();

}

} // namespace bashkir::log
