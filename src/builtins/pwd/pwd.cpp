#include "builtins/pwd/pwd.h"
#include <experimental/filesystem>
#include <iostream>

namespace fs = std::experimental::filesystem;

namespace bashkir::builtins
{

Pwd::Pwd(std::shared_ptr<BaseIO> nc_io)
    : io(std::move(nc_io)) {}

int Pwd::exec(const Command &cmd)
{
    this->io->writeStr(fs::current_path().c_str());
    return 0;
}

} // namespace bashkir::builtins
