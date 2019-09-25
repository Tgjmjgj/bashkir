// #include <experimental/filesystem>
// #include <numeric>
#include "builtins/echo/echo.h"
#include "global.h"

namespace fs = std::experimental::filesystem;

namespace bashkir::builtins
{

Echo::Echo() {}

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
    io.writeStr(echo_cmd);
    return 0;
}

} // namespace bashkir::builtins
