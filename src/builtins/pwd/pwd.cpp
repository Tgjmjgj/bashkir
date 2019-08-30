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
    std::cout << fs::current_path().c_str() << std::endl;
    return 0;
}

} // namespace bashkir::builtins
