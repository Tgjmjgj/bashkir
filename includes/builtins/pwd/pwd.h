#pragma once
#include <string>
#include <vector>
#include "builtins/Builtin.h"

namespace builtins
{
    class Pwd : public BuiltIn
    {
    public:
        int exec(std::vector<std::string> args);
    };

} // namespace builtins
