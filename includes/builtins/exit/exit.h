#pragma once
#include <deps.h>
// #include <memory>
#include "builtins/Builtin.h"

namespace bashkir::builtins
{

class Exit : public BuiltIn
{
public:
    ~Exit() {}
    int exec(const Command &cmd);
};

} // namespace bashkir::builtins
