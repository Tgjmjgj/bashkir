#include "builtins/exit/exit.h"
#include "global.h"

namespace bashkir::builtins
{

int Exit::exec(const Command &cmd)
{
    log::to->Info("Exit attempt!");
    exit(0);
    return 0;
}

} // namespace bashkir::builtins
