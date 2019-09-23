#pragma once
#include <memory>
#include "builtins/Builtin.h"

namespace bashkir::builtins
{

class Echo : public BuiltIn
{
public:
    Echo();
    ~Echo() {}
    int exec(const Command &cmd);
};

} // namespace bashkir::builtins
