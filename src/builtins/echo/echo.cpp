#include "builtins/echo/echo.h"
#include <experimental/filesystem>
#include <numeric>
#include <iostream>

namespace fs = std::experimental::filesystem;

namespace bashkir::builtins
{

Echo::Echo(std::shared_ptr<BaseIO> nc_io)
    : io(std::move(nc_io)) {}

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
    this->io->writeStr(echo_cmd);
    return 0;
}

} // namespace bashkir::builtins
