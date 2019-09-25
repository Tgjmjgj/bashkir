#pragma once
#include <deps.h>
// #include <memory>
#include "builtins/Builtin.h"
#include "BuiltinRegistry.h"

namespace bashkir::builtins
{

class Type : public BuiltIn
{
private:
    std::weak_ptr<BuiltinRegistry> builtins;
public:
    Type(std::weak_ptr<BuiltinRegistry> reg);
    ~Type() {}
    int exec(const Command &cmd);
};

} // namespace bashkir::builtins
