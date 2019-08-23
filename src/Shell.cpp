#include <iostream>
#include <unistd.h>
#include <map>
#include <experimental/filesystem>
#include "parser/BashkirCmdParser.h"
#include "exec/Executor.h"
#include "Shell.h"
#include "builtins/cd/cd.h"
#include "builtins/pwd/pwd.h"
#include "util/pathutil.h"

namespace fs = std::experimental::filesystem;

namespace bashkir
{

Shell::Shell()
{
    fs::current_path(getenv("HOME"));
    this->init();
}

Shell::~Shell()
{
}

void Shell::init()
{
    this->cmdParser = new BashkirCmdParser();
    if (this->registerBuiltin("cd", new builtins::Cd()) == -1)
        std::cerr << "Error with register builtin 'cd'" << std::endl;
    if (this->registerBuiltin("pwd", new builtins::Pwd()) == -1)
        std::cerr << "Error with register builtin 'pwd'" << std::endl;
}

int Shell::registerBuiltin(std::string name, builtins::BuiltIn *handler)
{
    this->builtinMap.insert_or_assign(name, handler);
    return 0;
}

builtins::BuiltIn *Shell::findBuiltin(std::string name)
{
    std::map<std::string, builtins::BuiltIn *>::iterator it = this->builtinMap.find(name);
    return it == this->builtinMap.end() ? nullptr : it->second;
}

int Shell::run()
{
    while (true)
    {
        std::string input = this->waitInput();
        auto cmds = this->cmdParser->parse(input);
        if (cmds.size() == 0)
            continue;
        builtins::BuiltIn *builtin = this->findBuiltin(cmds[0].exe);
        if (builtin != nullptr)
            builtin->exec(cmds[0].args);
        else
        {
            Executor *exec = new Executor();
            exec->execute(cmds[0]);
            exec->waitSubproc();
        }
    }
    return 0;
}

std::string Shell::waitInput()
{
    this->writePrefix();
    std::string buffer = std::string();
    std::getline(std::cin, buffer);
    return buffer;
}

void Shell::writePrefix()
{
    std::string cPath = fs::current_path().c_str();
    cPath = utils::fullToHomeRel(cPath);
    std::cout << "paradox> " << cPath << " $ ";
}

} // namespace bashkir
