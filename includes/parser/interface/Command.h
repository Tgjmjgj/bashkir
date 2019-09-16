#pragma once
#include <vector>
#include <string>

namespace bashkir
{

enum PipeFlow
{
    NORMAL,
    TO_RIGHT,
    TO_FILE,
    TO_FILE_APPEND,
    FILE_FROM_RIGHT
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
    PipeFlow io = NORMAL;
    std::vector<EnvVar> env;
};

} // namespace bashkir
