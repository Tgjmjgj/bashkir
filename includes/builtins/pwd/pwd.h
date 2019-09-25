#pragma once
#include <deps.h>
// #include <memory>
#include "builtins/Builtin.h"

namespace bashkir::builtins
{

class Pwd : public BuiltIn
{
public:
    Pwd();
    ~Pwd() {}
    int exec(const Command &cmd);
};

} // namespace bashkir::builtins
