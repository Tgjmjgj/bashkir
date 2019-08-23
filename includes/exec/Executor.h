#pragma once
#include "parser/interface/Command.h"

namespace bashkir
{

class Executor
{
public:
    int in;
    int out;

    Executor();
    ~Executor();
    int execute(const Command &cmd);
    void waitSubproc() const;

private:
    void createPipe();
    void closePipe();
};

} // namespace bashkir
