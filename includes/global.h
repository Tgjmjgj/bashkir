#pragma once
#include <memory>
#include "logger/interface/BaseLogger.h"
#include "logger/SpdFileLogger.h"

namespace bashkir::log
{

std::unique_ptr<BaseLogger> to = std::make_unique<SpdFileLogger>();

} // namespace bashkir::log
