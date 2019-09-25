#pragma once
#include "deps.h"
// #include <memory>
// #include <vector>
#include "parser/interface/Command.h"
#include "wrappers/NCurses.h"

namespace bashkir
{

class Executor
{
private:
    int in;
    int out;
    int err;
    std::vector<int> all_pipes;

    int pid;
    
public:

    Executor(int in=STDIN_FILENO, int out=STDOUT_FILENO, int err=STDERR_FILENO, const std::vector<int> &pp = {});
    int execute(const Command &cmd);
    void waitSubproc() const;
};

} // namespace bashkir
