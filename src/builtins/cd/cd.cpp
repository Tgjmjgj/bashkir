#include <iostream>
#include <experimental/filesystem>
#include "builtins/cd/cd.h"

namespace fs = std::experimental::filesystem;

namespace builtins
{

int Cd::exec(std::vector<std::string> args)
{
    if (args.empty())
    {
        std::cout << "Command 'cd' must receive at least 1 argument." << std::endl;
        return 0;
    }
    std::error_code err;
    std::string newPath = fs::current_path().append(args[0].c_str());
    fs::current_path(newPath, err);
    if (err.value() != 0)
    {
        std::cout << err.message() << std::endl;
        return -1;
    }
    return 0;
}

} // namespace builtins
