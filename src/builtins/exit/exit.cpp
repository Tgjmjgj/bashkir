#include "builtins/exit/exit.h"

namespace bashkir::builtins
{

int Exit::exec(const Command &cmd)
{
    exit(0);
    return 0;
}

} // namespace bashkir::builtins
