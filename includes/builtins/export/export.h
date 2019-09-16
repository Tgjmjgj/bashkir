#pragma once
#include <memory>
#include "builtins/Builtin.h"
#include "io/interface/BaseIO.h"

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
