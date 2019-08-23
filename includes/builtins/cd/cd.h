#pragma once
#include "builtins/Builtin.h"
#include <experimental/filesystem>
#include <string>
#include <vector>
#include <map>

namespace bashkir::builtins
{

class Cd : public BuiltIn
{
private:
    std::experimental::filesystem::path prev_dir;
    std::experimental::filesystem::path current_dir;
    std::map<std::string, std::experimental::filesystem::path> checkpoints;

public:
    Cd();
    ~Cd() {}
    int exec(const std::vector<std::string> &args);

private:
    std::experimental::filesystem::path evaluatePath(const std::string &path_arg) const;
    int changePath(const std::experimental::filesystem::path &new_path);
    void setCheckpoint(const std::string &name);
    std::experimental::filesystem::path getCheckpoint(const std::string &name) const noexcept(false);
    bool isCheckpoint(const std::string &name) const;
};

} // namespace bashkir::builtins
