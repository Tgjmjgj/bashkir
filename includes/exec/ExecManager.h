#pragma once
#include <vector>
#include <memory>
#include <fstream>
#include "BuiltinRegistry.h"
#include "parser/interface/Command.h"

namespace bashkir
{

struct FileMode
{
    const std::string filename;
    std::ios_base::openmode mode;
    FileMode(const std::string &name, std::ios_base::openmode md)
        : filename(name), mode(md) {}
};

struct FilesRedirect
{
    int pipe_in;
    std::vector<FileMode> files;
};

class ExecManager
{
private:
    std::shared_ptr<BaseIO> io;
    std::shared_ptr<BuiltinRegistry> builtins;
public:
    ExecManager(std::shared_ptr<BaseIO> nc_io, std::shared_ptr<BuiltinRegistry> reg);
    int execute(std::vector<Command> cmds);
private:
    void writeToFilesInSubprocess(const FilesRedirect& redir, std::vector<int> all_pipes);
};

} // namespace bashkir
