#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/wait.h>
#include <cstring>
#include "exec/Executor.h"
#include "util/convutil.h"

namespace bashkir
{

Executor::Executor()
{
    this->in = -1;
    this->out = -1;
}

int Executor::execute(Command &cmd)
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
        cmd.args.insert(cmd.args.begin(), cmd.exe);
        char* const* args = util::NullTerminatedCStrArr(cmd.args);
        int errCode = execvp(cmd.exe.c_str(), args);
        if (errCode == - 1)
        {
            switch (errno)
            {
            case 2:
                std::cout << "Command '" << cmd.exe << "' not found." << std::endl;
                break;
            default:
                std::cerr << cmd.exe << " return error code " << errno << ": " << std::strerror(errno) << std::endl;
                break;
            }
        }
        return 0;
    }
}

void Executor::waitSubproc()
{
    wait(NULL);
}

void Executor::createPipe()
{
    int *pipeIds = new int[2];
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

} // namespace bashkir
