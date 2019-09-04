#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/wait.h>
#include <cstring>
#include "exec/Executor.h"
#include "util/convutil.h"

namespace bashkir
{

Executor::Executor(std::shared_ptr<BaseIO> nc_io)
    : io(nc_io), in(-1), out(-1) {}

int Executor::execute(const Command &cmd)
{
    // this->createPipe();
    // dup2(this->in, STDIN_FILENO);
    // dup2(STDOUT_FILENO, this->out);
    const __pid_t child_id = fork();
    if (child_id)
    {
        if (child_id == -1)
        {
            this->io->writeStr("Something get wrongs!");
        }
        return child_id;
    }
    else
    {
        // dup2(this->in, STDIN_FILENO);
        // dup2(this->out, STDOUT_FILENO);
        char *const *args = util::createExecArgs(cmd.exe, cmd.args);
        int err_code = execvp(cmd.exe.c_str(), args);
        if (err_code == -1)
        {
            switch (errno)
            {
            case 2:
                this->io->writeStr("Command '" + cmd.exe + "' not found.");
                break;
            default:
                this->io->writeStr(cmd.exe + " return error code " + std::to_string(errno) + ": " + std::strerror(errno));
                break;
            }
        }
        return 0;
    }
}

void Executor::waitSubproc() const
{
    wait(NULL);
}

void Executor::createPipe()
{
    int *pipe_ids = new int[2];
    pipe(pipe_ids);
    this->in = pipe_ids[0];
    this->out = pipe_ids[1];
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
