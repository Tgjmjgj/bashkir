#pragma once
#include <memory>
#include "builtins/Builtin.h"
#include "io/interface/BaseIO.h"

namespace bashkir::builtins
{

class Pwd : public BuiltIn
{
private:
    std::shared_ptr<BaseIO> io;
public:
    Pwd(std::shared_ptr<BaseIO> nc_io);
    ~Pwd() {}
    int exec(const Command &cmd);
};

} // namespace bashkir::builtins
