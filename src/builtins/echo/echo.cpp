#include "builtins/echo/echo.h"
#include <experimental/filesystem>
#include <numeric>
#include <iostream>

namespace fs = std::experimental::filesystem;

namespace bashkir::builtins
{

int Echo::exec(const Command &cmd)
{
    std::string echo_cmd = std::accumulate(
        cmd.args.begin(),
        cmd.args.end(),
        std::string(),
        [](std::string accum, std::string next) {
            return accum + next;
        }
    );
    std::cout << echo_cmd << std::endl;
    return 0;
}

} // namespace bashkir::builtins
