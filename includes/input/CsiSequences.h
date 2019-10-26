#pragma once
#include "deps.h"
// #include <string>
// #include <regex>
// #include <vector>

namespace bashkir::csi
{
// \x1B - ESC
const std::regex full_csi("(?:\x1B[\x40-\x5F]|[\x80-\x9F])[\x30-\x3F]*[\x20-\x2F]*[\x40-\x7E]");
const std::vector<std::regex> csi_parts = {
    std::regex("\x1B|[\x80-\x9F]"),
    std::regex("\x1B[\x40-\x5F]|[\x80-\x9F]"),
    std::regex("(?:\x1B[\x40-\x5F]|[\x80-\x9F])[\x30-\x3F]*"),
    std::regex("(?:\x1B[\x40-\x5F]|[\x80-\x9F])[\x30-\x3F]*[\x20-\x2F]*")
};

const std::regex BS_UP_ARROW("\x1B\\[[1-9]{0,3}A");
const std::regex BS_DOWN_ARROW("\x1B\\[[1-9]{0,3}B");
const std::regex BS_RIGHT_ARROW("\x1B\\[[1-9]{0,3}C");
const std::regex BS_LEFT_ARROW("\x1B\\[[1-9]{0,3}D");
const std::regex BS_SHIFT_UP_ARROW("\x1B\\[[1-9]{0,3};2A");
const std::regex BS_SHIFT_DOWN_ARROW("\x1B\\[[1-9]{0,3};2B");
const std::regex BS_SHIFT_RIGHT_ARROW("\x1B\\[[1-9]{0,3};2C");
const std::regex BS_SHIFT_LEFT_ARROW("\x1B\\[[1-9]{0,3};2D");
const std::regex BS_DELETE("\x1B\\[3~");

const std::regex BS_KEY_ENTER("\r");
const std::regex BS_KEY_BACKSPACE("\177");
const std::regex BS_KEY_CTRL_C("\3");
const std::regex BS_KEY_TAB("\t");

enum class CsiMatchStatus
{
    NOT_MATCH,
    FULL_MATCH,
    PARTIAL_MATCH
};

CsiMatchStatus matchCsi(const std::string &str);

} // namespace bashkir::csi
