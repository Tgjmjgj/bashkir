// #include <experimental/filesystem>
#include "builtins/pwd/pwd.h"
#include "global.h"

namespace fs = std::experimental::filesystem;

namespace bashkir::builtins
{

Pwd::Pwd() {}

int Pwd::exec(const Command &cmd)
{
    io.writeStr(fs::current_path().c_str());
    return 0;
}

} // namespace bashkir::builtins
