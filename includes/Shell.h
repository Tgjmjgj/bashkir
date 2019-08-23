#pragma once
#include <string>
#include <map>
#include "parser/interface/ICmdParser.h"
#include "builtins/Builtin.h"

namespace bashkir
{

class Shell
{
private:
    ICmdParser *cmdParser;
    std::string dir;
    std::map<std::string, builtins::BuiltIn *> builtinMap;

public:
    Shell();
    ~Shell();
    int run();

private:
    void init();
    int registerBuiltin(std::string name, builtins::BuiltIn *handler);
    builtins::BuiltIn *findBuiltin(std::string name);
    void writePrefix();
    std::string waitInput();
};

} // namespace bashkir
