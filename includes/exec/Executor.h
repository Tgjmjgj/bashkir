#pragma once
#include <memory>
#include "parser/interface/Command.h"
#include "io/interface/BaseIO.h"
#include "wrappers/NCurses.h"

namespace bashkir
{

class Executor
{
private:
    std::shared_ptr<BaseIO> io;
    int pid;
    
public:
    int in;
    int out;
    int err;

    Executor(std::shared_ptr<BaseIO> nc_io, int in=STDIN_FILENO, int out=STDOUT_FILENO, int err=STDERR_FILENO);
    int execute(const Command &cmd);
    void waitSubproc() const;
};

} // namespace bashkir
