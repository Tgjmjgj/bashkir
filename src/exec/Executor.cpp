#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/wait.h>
#include <cstring>
#include "exec/Executor.h"
#include "util/convutil.h"

namespace bashkir
{

Executor::Executor(std::shared_ptr<BaseIO> nc_io, int i, int o, int e)
    : io(std::move(nc_io)), in(i), out(o), err(e) {}

int Executor::execute(const Command &cmd)
{
    const __pid_t child_id = fork();
    if (child_id) // parent process
    {
        if (child_id == -1)
        {
            this->io->writeStr("Something get wrongs!");
        }
        this->pid = child_id;
        return child_id;
    }
    else // child process
    {
        if (this->out != STDOUT_FILENO)
        {
            dup2(this->out, STDOUT_FILENO);
            close(this->out);
            close(this->out - 1);
        }
        if (this->in != STDIN_FILENO)
        {
            dup2(this->in, STDIN_FILENO);
            close(this->in);
            close(this->in + 1);
        }
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

} // namespace bashkir
