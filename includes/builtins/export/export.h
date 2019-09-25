#pragma once
#include <deps.h>
// #include <memory>
#include "builtins/Builtin.h"

namespace bashkir::builtins
{

class Export : public BuiltIn
{
public:
    Export();
    ~Export() {}
    int exec(const Command &cmd);
};

} // namespace bashkir::builtins
