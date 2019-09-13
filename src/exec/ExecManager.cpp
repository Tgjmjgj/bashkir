#include <unistd.h>
#include <sys/wait.h>
#include "exec/ExecManager.h"
#include "exec/Executor.h"
#include "global.h"

namespace bashkir
{

ExecManager::ExecManager(std::shared_ptr<BaseIO> nc_io, std::shared_ptr<BuiltinRegistry> reg)
    : io(std::move(nc_io)), builtins(std::move(reg)) {}

int ExecManager::execute(std::vector<Command> cmds)
{
    std::vector<int> pipes;
    std::vector<Executor> subprocs;
    int next_in = STDIN_FILENO;
    if (LOG_L2) log::to->Info("RUN COMMANDS:");
    for (const Command &cmd : cmds)
    {
        int in = next_in;
        int out = STDOUT_FILENO;
        int err = STDERR_FILENO;
        if (cmd.io == PipeFlow::TO_RIGHT)
        {
            int pipe_ids[2];
            pipe(pipe_ids);
            pipes.push_back(pipe_ids[0]);
            pipes.push_back(pipe_ids[1]);
            out = pipe_ids[1];
            next_in = pipe_ids[0];
        }
        else
        {
            next_in = STDIN_FILENO;
        }
        auto builtin = this->builtins->findBuiltin(cmd.exe);
        if (builtin != nullptr)
        {
            builtin.get()->exec(cmd);
        }
        else
        {
            Executor exec(this->io, in, out, err, pipes);
            subprocs.push_back(exec);
            exec.execute(cmd);
        }
    }
    for (int pipe_id : pipes)
    {
        close(pipe_id);
    }
    for (Executor &ex : subprocs)
    {
        ex.waitSubproc();
    }
    return 0;
}

} // namespace bashkir
