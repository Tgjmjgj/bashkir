#include <experimental/filesystem>
#include <iostream>
#include "builtins/export/export.h"
#include "util/strutil.h"

namespace fs = std::experimental::filesystem;

namespace bashkir::builtins
{

Export::Export() {}

int Export::exec(const Command &cmd)
{
    for (const std::string &arg : cmd.args)
    {
        std::tuple<std::string, std::string> parts = util::splitInHalf(arg, '=');
        if (std::get<1>(parts) != "")
        {
            std::string name = std::get<0>(parts).c_str();
            std::string value = std::get<1>(parts).c_str();
            setenv(name.c_str(), value.c_str(), 1);
        }
    }
    return 0;
}

} // namespace bashkir::builtins
