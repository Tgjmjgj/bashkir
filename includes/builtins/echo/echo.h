#pragma once
#include "builtins/Builtin.h"

namespace bashkir::builtins
{

class Echo : public BuiltIn
{
public:
    ~Echo() {}
    int exec(const Command &cmd);
};

} // namespace bashkir::builtins
