#include "exec/Executor.h"
#include <unistd.h>
#include <iostream>
#include <string.h>

Executor::Executor()
{
    this->in = -1;
    this->out = -1;
}

int Executor::execute(Command& cmd)
{
    // this->createPipe();
    // dup2(this->in, STDIN_FILENO);
    // dup2(STDOUT_FILENO, this->out);
    const __pid_t childId = fork();
    if (childId) 
    {
        if (childId == -1)
            std::cout << "Something get wrongs!" << std::endl;
        return childId;
    }
    else
    {
        // dup2(this->in, STDIN_FILENO);
        // dup2(this->out, STDOUT_FILENO);
        std::cout << "child" << std::endl;

        execvp(cmd.exe, cmd.args);
        return 0;
    }
}

void Executor::createPipe()
{
    int* pipeIds = new int[2];
    pipe(pipeIds);
    this->in = pipeIds[0];
    this->out = pipeIds[1];
}

void Executor::closePipe()
{
    close(this->in);
    close(this->out);
}

Executor::~Executor()
{
    this->closePipe();
}
