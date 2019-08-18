#pragma once
#include "parser/interface/Command.h"

class Executor
{
public:
    int out;
    int in;

    Executor();
    ~Executor();
    int execute(Command& cmd);
private:
    void createPipe();
    void closePipe();

};
