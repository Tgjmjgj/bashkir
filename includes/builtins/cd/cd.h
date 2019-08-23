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
    std::experimental::filesystem::path prevDir;
    std::experimental::filesystem::path currentDir;
    std::map<std::string, std::experimental::filesystem::path> checkpoints;
public:
    Cd();
    int exec(std::vector<std::string> args);
private:
    std::experimental::filesystem::path evaluatePath(std::string pathArg);
    int changePath(std::experimental::filesystem::path toGo);
    void setCheckpoint(std::string name);
    std::experimental::filesystem::path getCheckpoint(std::string name);
    bool isCheckpoint(std::string name);
};

} // namespace builtins
