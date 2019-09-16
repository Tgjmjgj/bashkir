#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/wait.h>
#include <cstring>
#include "exec/Executor.h"
#include "util/convutil.h"
#include "global.h"

namespace bashkir
{

Executor::Executor(std::shared_ptr<BaseIO> nc_io, int i, int o, int e, const std::vector<int> &pp)
    : io(std::move(nc_io)), in(i), out(o), err(e), all_pipes(pp) {}

int Executor::execute(const Command &cmd)
{
    if (LOG_L2) log::to->Info("  [" + std::to_string(this->in) + "]=======> " + cmd.exe + " =======>[" + std::to_string(this->out) + "]");
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
        }
        if (this->in != STDIN_FILENO)
        {
            dup2(this->in, STDIN_FILENO);
        }
        for (int pipe : this->all_pipes)
        {
            close(pipe);
        }
        for (const EnvVar &env : cmd.env)
        {
            setenv(env.name.c_str(), env.value.c_str(), 1);
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
