#pragma once
#include <experimental/filesystem>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <memory>
#include "builtins/Builtin.h"
#include "io/interface/BaseIO.h"

namespace bashkir::builtins
{

class Cd : public BuiltIn
{
private:
    std::experimental::filesystem::path prev_dir;
    std::experimental::filesystem::path current_dir;
    std::map<std::string, std::experimental::filesystem::path> checkpoints;
    std::stack<std::experimental::filesystem::path> dir_stack;
    std::shared_ptr<BaseIO> io;
public:
    Cd(std::shared_ptr<BaseIO> nc_io);
    ~Cd() {}
    int exec(const Command &cmd);

private:
    int exec_cd(const std::vector<std::string> &args);
    int exec_pushd(const std::vector<std::string> &args);
    int exec_popd(const std::vector<std::string> &args);
    std::experimental::filesystem::path evaluatePath(const std::string &path_arg) const;
    int changePath(const std::experimental::filesystem::path &new_path);
    void setCheckpoint(const std::string &name);
    std::experimental::filesystem::path getCheckpoint(const std::string &name) const noexcept(false);
    bool isCheckpoint(const std::string &name) const;
};

} // namespace bashkir::builtins
