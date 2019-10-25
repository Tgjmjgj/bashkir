#pragma once
#include "deps.h"
#include "input/interface/InputOption.h"
#include "input/InputSpecialKey.h"
// #include <string>
// #include <vector>
// #include <regex>

namespace bashkir::csi
{
// \x1B - ESC
const std::regex full_csi("(?:\x1B[\x40-\x5F]|[\x80-\x9F])[\x30-\x3F]*[\x20-\x2F]*[\x40-\x7E]");
const std::regex part_csi("(?:\x1B|\x1B[\x40-\x5F]|[\x80-\x9F]|(?:\x1B[\x40-\x5F]|[\x80-\x9F])[\x30-\x3F]*|(?:\x1B[\x40-\x5F]|[\x80-\x9F])[\x30-\x3F]*[\x20-\x2F]*)");

const std::string BS_UP_ARROW = "\033[A";
const std::string BS_DOWN_ARROW = "\033[B";
const std::string BS_RIGHT_ARROW = "\033[C";
const std::string BS_LEFT_ARROW = "\033[D";
const std::string BS_SHIFT_UP_ARROW = "\033[1;2A";
const std::string BS_SHIFT_DOWN_ARROW = "\033[1;2B";
const std::string BS_SHIFT_RIGHT_ARROW = "\033[1;2C";
const std::string BS_SHIFT_LEFT_ARROW = "\033[1;2D";
const std::string BS_DELETE = "\033[3~";

const char BS_KEY_ENTER = '\r';
const char BS_KEY_BACKSPACE = '\177';
const char BS_KEY_CTRL_C = '\3';
const char BS_KEY_TAB = '\t';

enum class CsiMatchStatus
{
    NOT_MATCH,
    FULL_MATCH,
    PARTIAL_MATCH
};

CsiMatchStatus matchCsi(const std::string &str);

enum class CommandStart
{
    NONE,       /* {no special start command prefix} */
    ESC,        /* ESC      -- Escape Code */
    CSI,        /* ESC [    -- Control Sequence Introducer */
    SS2,        /* ESC N    -- Single Shift Two */
    SS3,        /* ESC O    -- Single Shift Three */
    DCS,        /* ESC P    -- Device Control String */
    ST,         /* ESC \    -- String Terminator */
    OSC,        /* ESC ]    -- Operating System Command */
    SOS,        /* ESC X    -- Start of String */
    PM,         /* ESC ^    -- Privacy Message */
    APC,        /* ESC _    -- Application Program Command */
    RIS         /* ESC c    -- Reset to Initial State */
};

class CsiSequence
{
public:
    const CommandStart start;
    const std::vector<uint> params;
    const std::vector<char> mid;
    const char term;
    CsiSequence(const CommandStart &strt, const std::vector<uint> &prms, const std::vector<uint> &interm, char trm);
    const std::string getStrWithoutModifiers() const;
};

const CsiSequence convertToCsiObject(const std::string &csi_str);

InputSpecialKey extractOptions(const std::string &csi_seq);

} // namespace bashkir::csi
