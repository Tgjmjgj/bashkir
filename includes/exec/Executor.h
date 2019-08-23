#pragma once
#include "parser/interface/Command.h"

namespace bashkir
{

class Executor
{
public:
    int out;
    int in;

    Executor();
    ~Executor();
    int execute(Command &cmd);
    void waitSubproc();

private:
    void createPipe();
    void closePipe();
};

} // namespace bashkir
