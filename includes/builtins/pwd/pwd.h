#pragma once
#include "builtins/Builtin.h"

namespace bashkir::builtins
{

class Pwd : public BuiltIn
{
public:
    ~Pwd() {}
    int exec(const Command &cmd);
};

} // namespace bashkir::builtins
