#include "builtins/pwd/pwd.h"
#include <experimental/filesystem>
#include <iostream>

namespace fs = std::experimental::filesystem;

namespace bashkir::builtins
{

int Pwd::exec(const Command &cmd)
{
    std::cout << fs::current_path().c_str() << std::endl;
    return 0;
}

} // namespace bashkir::builtins
