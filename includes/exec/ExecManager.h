#pragma once
#include <vector>
#include <memory>
#include "BuiltinRegistry.h"
#include "parser/interface/Command.h"

namespace bashkir
{

class ExecManager
{
private:
    std::shared_ptr<BaseIO> io;
    std::shared_ptr<BuiltinRegistry> builtins;
public:
    ExecManager(std::shared_ptr<BaseIO> nc_io, std::shared_ptr<BuiltinRegistry> reg);
    int execute(std::vector<Command> cmds);

};

} // namespace bashkir
