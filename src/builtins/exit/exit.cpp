#include "builtins/exit/exit.h"
#include "global.h"

namespace bashkir::builtins
{

int Exit::exec(const Command &cmd)
{
    log::to->Info("Exit attempt!");
    io.writeStr("bashkir> Good By!");
    exit(0);
    return 0;
}

} // namespace bashkir::builtins
