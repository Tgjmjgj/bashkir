// #include <unistd.h>
// #include <string.h>
// #include <sys/wait.h>
// #include <string.h>
#include "exec/Executor.h"
#include "util/convutil.h"
#include "exceptions/ExitException.h"
#include "global.h"

namespace bashkir
{

Executor::Executor(int i, int o, int e, const std::vector<int> &pp)
    : in(i), out(o), err(e), all_pipes(pp) {}

int Executor::execute(const Command &cmd)
{
    if (log::Lev2()) log::to->Info("  [" + std::to_string(this->in) + "]=======> " + cmd.exe + " =======>[" + std::to_string(this->out) + "]");
    const __pid_t child_id = fork();
    if (child_id) // parent process
    {
        if (child_id == -1)
        {
            io.writeStr("Something get wrongs!");
        }
        this->pid = child_id;
        return child_id;
    }
    else // child process
    {
        // We need do this explicitly, because SIGINT is disabled in the main parent process
        signal(SIGINT, global::allowCtrlC);
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
                io.writeStr("Command '" + cmd.exe + "' not found.");
                break;
            default:
                io.writeStr(cmd.exe + " return error code " + std::to_string(errno) + ": " + std::strerror(errno));
                break;
            }
        }
        throw exc::ExitException(err_code);
        return 1;
    }
}

void Executor::waitSubproc() const
{
    wait(NULL);
}

} // namespace bashkir
