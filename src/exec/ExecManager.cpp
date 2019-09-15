#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include "exec/ExecManager.h"
#include "exec/Executor.h"
#include "global.h"

#define SFILE(x) (x == PipeFlow::TO_FILE || x == PipeFlow::TO_FILE_APPEND)

namespace bashkir
{

ExecManager::ExecManager(std::shared_ptr<BaseIO> nc_io, std::shared_ptr<BuiltinRegistry> reg)
    : io(std::move(nc_io)), builtins(std::move(reg)) {}

int ExecManager::execute(std::vector<Command> cmds)
{
    std::vector<int> pipes, file_pipes;
    std::vector<Executor> subprocs;
    int next_in = STDIN_FILENO;
    if (LOG_L2) log::to->Info("RUN COMMANDS:");
    std::vector<FilesRedirect> mul_files;
    for (std::size_t i = 0; i < cmds.size();)
    {
        std::size_t step = 1;
        int in = next_in;
        int out = STDOUT_FILENO;
        int err = STDERR_FILENO;
        if (cmds[i].io == PipeFlow::TO_RIGHT)
        {
            int pipe_ids[2];
            pipe(pipe_ids);
            pipes.push_back(pipe_ids[0]);
            pipes.push_back(pipe_ids[1]);
            out = pipe_ids[1];
            next_in = pipe_ids[0];
        } 
        else if (SFILE(cmds[i].io))
        {
            FilesRedirect redir;
            for (std::size_t j = i; j < cmds.size() - 1 && SFILE(cmds[j].io); ++j)
            {
                auto mode = cmds[j].io == PipeFlow::TO_FILE ? std::ios_base::out : std::ios_base::app;
                redir.files.push_back(FileMode(cmds[j + 1].exe, mode));
                for (std::string &arg : cmds[j + 1].args)
                {
                    redir.files.push_back(FileMode(arg, mode));
                }
                ++step;
            }
            int pipe_ids[2];
            pipe(pipe_ids);
            file_pipes.push_back(pipe_ids[0]);
            file_pipes.push_back(pipe_ids[1]);
            out = pipe_ids[1];
            next_in = STDIN_FILENO;
            redir.pipe_in = pipe_ids[0];
            mul_files.push_back(redir);
        }
        else
        {
            next_in = STDIN_FILENO;
        }
        auto builtin = this->builtins->findBuiltin(cmds[i].exe);
        if (builtin != nullptr)
        {
            builtin.get()->exec(cmds[i]);
        }
        else
        {
            Executor exec(this->io, in, out, err, pipes);
            subprocs.push_back(exec);
            exec.execute(cmds[i]);
        }
        i += step;
    }
    for (int pipe_id : pipes)
    {
        close(pipe_id);
    }
    for (Executor &ex : subprocs)
    {
        ex.waitSubproc();
    }
    for (FilesRedirect &redir : mul_files)
    {
        this->writeToFilesInSubprocess(redir, file_pipes);
    }
    for (int pipe_id : file_pipes)
    {
        close(pipe_id);
    }
    return 0;
}

void ExecManager::writeToFilesInSubprocess(const FilesRedirect &redir, std::vector<int> all_file_pipes)
{
    const __pid_t child_id = fork();
    if (child_id == -1)
    {
        this->io->writeStr("Something get wrongs!");
    }
    else if (child_id == 0) // child process
    {
        log::to->Info("Start a child process with id " + std::to_string(child_id));
        for (const int pipe : all_file_pipes)
        {
            if (pipe != redir.pipe_in)
            {
                close(pipe);
            }
        }
        std::vector<std::ofstream*> files;
        for (const FileMode &file_info : redir.files)
        {
            try
            {
                std::ofstream *file = new std::ofstream();
                (*file).open(file_info.filename, file_info.mode);
                files.push_back(file);
            }
            catch(const std::exception& e)
            {
                this->io->error("Error with opening file " + file_info.filename);
            }
        }
        const int buf_size = 64;
        char buf[buf_size];
        log::to->Info("Write to file:");
        while (read(redir.pipe_in, &buf, sizeof(buf)) > 0)
        {
            for (std::ofstream *filestream : files)
            {
                (*filestream) << buf;
                log::to->Info(buf);
            }
        }
        for (std::ofstream *filestream : files)
        {
            (*filestream).close();
        }
        log::to->Info("All ok. It reachs the end.");
        close(redir.pipe_in);
        global::restore_term_atexit = false;
        exit(0);
    }
}

} // namespace bashkir
