#include "builtins/pwd/pwd.h"
#include <experimental/filesystem>
#include <iostream>

namespace fs = std::experimental::filesystem;

namespace bashkir::builtins
{

int Pwd::exec(const std::vector<std::string> &args)
{
    std::cout << fs::current_path().c_str() << std::endl;
    return 0;
}

} // namespace bashkir::builtins
