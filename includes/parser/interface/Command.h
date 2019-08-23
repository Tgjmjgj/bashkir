#pragma once
#include <vector>
#include <string>

namespace bashkir
{

struct Command
{
    std::string exe;
    std::vector<std::string> args;
};

} // namespace bashkir
