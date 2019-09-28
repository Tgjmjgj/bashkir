#pragma once
#include "deps.h"
// #include <string>
// #include <vector>

namespace bashkir
{

enum PipeFlow : uint8_t
{
    NORMAL,
    TO_RIGHT,
    TO_FILE,
    TO_FILE_APPEND
};

struct EnvVar
{
    std::string name;
    std::string value;
};

struct Command
{
    std::string exe;
    std::vector<std::string> args;
    std::vector<EnvVar> env;
    PipeFlow io = PipeFlow::NORMAL;
};

inline bool isRedirToFile(PipeFlow pipe_mode)
{
    return pipe_mode == PipeFlow::TO_FILE || pipe_mode == PipeFlow::TO_FILE_APPEND;
}

} // namespace bashkir
