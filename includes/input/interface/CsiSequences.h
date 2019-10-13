#pragma once
#include "deps.h"
// #include <string>

namespace bashkir::csi
{

const std::string regex = "(?:\x1B[@-_]|[\x80-\x9F])[0-?]*[ -/]*[@-~]";

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

} // namespace bashkir::csi
