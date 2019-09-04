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

public:
    int in;
    int out;

    Executor(std::shared_ptr<BaseIO> nc_io);
    ~Executor();
    int execute(const Command &cmd);
    void waitSubproc() const;

private:
    void createPipe();
    void closePipe();
};

} // namespace bashkir
