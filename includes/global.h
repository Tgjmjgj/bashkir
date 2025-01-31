#pragma once
#include "deps.h"
// #include <memory>
// #include <termios.h>
// #include <sys/wait.h>
#include "logger/interface/BaseLogger.h"
#include "logger/SpdFileLogger.h"
#include "io/StreamIO.h"

namespace bashkir
{

inline StreamIO io;

namespace log
{

inline uint8_t log_level = 2;
inline std::unique_ptr<BaseLogger> to = std::make_unique<SpdFileLogger>();

inline bool Lev1() {
    return log_level > 0;
}
inline bool Lev2() {
    return log_level > 1;
}
inline bool Lev3() {
    return log_level > 2;
}

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

inline void disableCtrlC(int signum)
{
    NULL;
}

inline uint8_t bad_alloc_chain = 0;

} // namespace bashkir::global

} // namespace bashkir
