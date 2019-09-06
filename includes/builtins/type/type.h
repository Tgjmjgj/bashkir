#pragma once
#include <memory>
#include "builtins/Builtin.h"
#include "io/interface/BaseIO.h"
#include "BuiltinRegistry.h"

namespace bashkir::builtins
{

class Type : public BuiltIn
{
private:
    std::shared_ptr<BaseIO> io;
    std::weak_ptr<BuiltinRegistry> builtins;
public:
    Type(std::shared_ptr<BaseIO> nc_io, std::weak_ptr<BuiltinRegistry> reg);
    ~Type() {}
    int exec(const Command &cmd);
};

} // namespace bashkir::builtins
