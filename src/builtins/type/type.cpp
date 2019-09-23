#include <experimental/filesystem>
#include <iostream>
#include "builtins/type/type.h"
#include "util/pathutil.h"
#include "global.h"

namespace fs = std::experimental::filesystem;

namespace bashkir::builtins
{

Type::Type(std::weak_ptr<BuiltinRegistry> reg)
    : builtins(std::move(reg)) {}

int Type::exec(const Command &cmd)
{
    if (cmd.args.size() == 0)
    {
        return 0;
    }
    auto breg = this->builtins.lock();
    if (breg == nullptr)
    {
        io.writeStr("Something get wrong with a pointer on the Builtin registry.");
        return -1;
    }
    std::string name = cmd.args[0];
    auto builtin = breg->findBuiltin(name);
    if (builtin != nullptr)
    {
        io.writeStr(name + " is a shell builtin");
        return 0;
    }
    std::string full_file_name = util::resolveFileName(name);
    if (!full_file_name.empty())
    {
        io.writeStr(name + " is " + full_file_name);
        return 0;
    }
    io.writeStr("type: " + name + " not found");
    // TODO: alias
    return 0;
}

} // namespace bashkir::builtins
