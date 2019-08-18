#include "builtins/pwd/pwd.h"
#include <experimental/filesystem>
#include <iostream>

namespace fs = std::experimental::filesystem;

namespace builtins
{

int Pwd::exec(std::vector<std::string> args)
{
    std::cout << fs::current_path().c_str() << std::endl;
    return 0;
}

} // namespace builtins
