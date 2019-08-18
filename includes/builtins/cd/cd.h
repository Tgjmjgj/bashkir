#pragma once
#include "builtins/Builtin.h"
#include <string>
#include <vector>

namespace builtins
{

class Cd : public BuiltIn
{
public:
    int exec(std::vector<std::string> args);
};

} // namespace builtins
