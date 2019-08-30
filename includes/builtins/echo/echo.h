#pragma once
#include <memory>
#include "builtins/Builtin.h"
#include "io/interface/BaseIO.h"

namespace bashkir::builtins
{

class Echo : public BuiltIn
{
    std::shared_ptr<BaseIO> io;
public:
    Echo(std::shared_ptr<BaseIO> nc_io);
    ~Echo() {}
    int exec(const Command &cmd);
};

} // namespace bashkir::builtins
