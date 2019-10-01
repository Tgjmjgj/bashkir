// #include <unistd.h>
// #include <sys/wait.h>
// #include <fstream>
#include "exec/ExecManager.h"
#include "exec/Executor.h"
#include "global.h"

namespace bashkir
{

ExecManager::ExecManager(std::shared_ptr<BuiltinRegistry> reg)
    : builtins(std::move(reg)) {}

int ExecManager::execute(std::vector<Command> cmds)
{
    std::vector<int> pipes, file_pipes;
    std::vector<Executor> subprocs;
    int next_in = STDIN_FILENO;
    if (log::Lev2()) log::to->Info("RUN COMMANDS:");
    std::vector<FilesRedirect> mul_files;
    bool term_reset = false;
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
        else if (isRedirToFile(cmds[i].io))
        {
            FilesRedirect redir;
            for (std::size_t j = i; j < cmds.size() - 1 && isRedirToFile(cmds[j].io); ++j)
            {
                auto mode = cmds[j].io == PipeFlow::TO_FILE ? std::ios_base::out : std::ios_base::app;
                redir.files.push_back(FileMode(cmds[j + 1].exe, mode));
                for (std::string &arg : cmds[j + 1].args)
                {
                    redir.files.push_back(FileMode(arg, mode));
                }
                ++step;
            }
            if (redir.files.size() == 1)
            {
                int fd = open(
                    redir.files[0].filename.c_str(),
                    O_WRONLY | O_CREAT | (redir.files[0].mode == std::ios_base::out ? 0 : O_CREAT),
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH   // -rw-r--r-- or 644
                );
                if (fd == 0) {
                    std::string msg = "Cannot open file '" + redir.files[0].filename + '\'';
                    io.error(msg);
                    log::to->Warn(msg);
                }
                file_pipes.push_back(fd);
                pipes.push_back(fd);
                out = fd;
            }
            else
            {
                int pipe_ids[2];
                pipe(pipe_ids);
                file_pipes.push_back(pipe_ids[0]);
                file_pipes.push_back(pipe_ids[1]);
                pipes.push_back(pipe_ids[0]);
                pipes.push_back(pipe_ids[1]);
                out = pipe_ids[1];
                redir.pipe_in = pipe_ids[0];
                mul_files.push_back(redir);
            }
            next_in = STDIN_FILENO;
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
            if (!term_reset)
            {
                global::resetOriginalTermSettings();
                term_reset = true;
            }
            Executor exec(in, out, err, pipes);
            subprocs.push_back(exec);
            exec.execute(cmds[i]);
            // wait(NULL);
            // exec.waitSubproc();
        }
        i += step;
    }
    for (int pipe_id : pipes)
    {
        close(pipe_id);
    }
    for (Executor &child : subprocs)
    {
        wait(NULL);
    }
    for (FilesRedirect &redir : mul_files)
    {
        this->writeToFilesInSubprocess(redir, file_pipes);
    }
    for (int pipe_id : file_pipes)
    {
        close(pipe_id);
    }
    if (term_reset)
    {
        global::setBashkirTermSettings();
    }
    return 0;
}

void ExecManager::writeToFilesInSubprocess(const FilesRedirect &redir, std::vector<int> all_file_pipes)
{
    const __pid_t child_id = fork();
    if (child_id == -1)
    {
        io.writeStr("Something get wrongs!");
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
                io.error("Error with opening file " + file_info.filename);
            }
        }
        char buf;
        log::to->Info("Write to file:");
        while (read(redir.pipe_in, &buf, sizeof(buf)) > 0)
        {
            for (std::ofstream *filestream : files)
            {
                (*filestream) << buf;
            }
        }
        for (std::ofstream *filestream : files)
        {
            (*filestream).close();
            delete filestream;
        }
        log::to->Info("All ok. It reachs the end.");
        close(redir.pipe_in);
        // global::restore_term_atexit = false;
        exit(0);
    }
}

} // namespace bashkir
