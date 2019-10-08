#pragma once
#include "deps.h"
// #include <memory>
// #include <termios.h>
// #include <sys/wait.h>
#include "logger/SpdFileLogger.h"
#include "io/StreamIO.h"
#include "input/BlockConstructions.h"

namespace bashkir
{

inline StreamIO io;

namespace log
{

inline uint8_t log_level = 0;
inline SpdFileLogger to;

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

inline const Blocks blocksMeta;

// inline bool restore_term_atexit = true;
inline termios settings_original, settings_bashkir;
inline bool is_bashkir_term_mode = false;

inline bool setBashkirTermSettings()
{
    bool ret_code = tcsetattr(0, TCSANOW, &settings_bashkir) >= 0;
    if (ret_code)
    {
        is_bashkir_term_mode = true;
        // io.writeStr("Bashkir mode");
    }
    else
    {
        io.error("Error with setting new settings of terminal.");
    }
    return ret_code;
}

inline bool resetOriginalTermSettings()
{
    bool ret_code = tcsetattr(0, TCSANOW, &settings_original) >= 0;
    if (ret_code)
    {
        is_bashkir_term_mode = false;
        // io.writeStr("Default mode");
    }
    else
    {
        io.error("Error with resetting original settings of the terminal.");
    }
    return ret_code;
}

inline void atexit()
{
    if (is_bashkir_term_mode)
    {
        resetOriginalTermSettings();
    }
}

inline void antiZombie(int signum)
{
    wait(NULL);
}

} // namespace bashkir::global

} // namespace bashkir
