#pragma once
#include <memory>
#include <termios.h>
#include <sys/wait.h>
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

} // namespace bashkir::log

namespace global
{

inline bool restore_term_atexit = true;
inline termios settings_classic;
inline termios settings_bashkir;

inline bool bashkirTermSettings()
{
    return tcsetattr(0, TCSANOW, &settings_bashkir) >= 0;
    
}

inline bool classicTermSettings()
{
    return tcsetattr(0, TCSANOW, &settings_classic) >= 0;
}

inline void atexit()
{
    if (restore_term_atexit)
    {
        classicTermSettings();
    }
}

inline void antiZombie(int signum)
{
    wait(NULL);
}

} // namespace bashkir::global

} // namespace bashkir
