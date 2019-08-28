#pragma once
#include "parser/interface/Command.h"

namespace bashkir::builtins
{

class BuiltIn
{
public:
    virtual ~BuiltIn() = default;
    virtual int exec(const Command &cmd) = 0;
};

} // namespace bashkir::builtins
