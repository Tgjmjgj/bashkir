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

void Shell::init()
{
    this->parser = std::make_unique<BashkirCmdParser>();
    if (this->registerBuiltin("cd", std::make_shared<builtins::Cd>()) == -1)
    {
        std::cerr << "Error with register builtin 'cd'" << std::endl;
    }
    if (this->registerBuiltin("pwd", std::make_shared<builtins::Pwd>()) == -1)
    {
        std::cerr << "Error with register builtin 'pwd'" << std::endl;
    }
}

int Shell::registerBuiltin(const std::string &name, const std::shared_ptr<builtins::BuiltIn> &&handler)
{
    this->builtins.insert_or_assign(name, handler);
    return 0;
}

std::shared_ptr<builtins::BuiltIn> Shell::findBuiltin(const std::string &name) const
{
    auto it = this->builtins.find(name);
    return it == this->builtins.end() ? nullptr : it->second;
}

int Shell::run()
{
    while (true)
    {
        const std::string input = this->waitInput();
        auto cmds = this->parser->parse(input);
        if (cmds.size() == 0)
        {
            continue;
        }
        auto builtin = this->findBuiltin(cmds[0].exe);
        if (builtin != nullptr)
        {
            builtin.get()->exec(cmds[0].args);
        }
        else
        {
            std::unique_ptr<Executor> exec = std::make_unique<Executor>();
            exec->execute(cmds[0]);
            exec->waitSubproc();
        }
    }
    return 0;
}

std::string Shell::waitInput() const
{
    this->writePrefix();
    std::string buffer = std::string();
    std::getline(std::cin, buffer);
    return buffer;
}

void Shell::writePrefix() const
{
    std::string cPath = fs::current_path().c_str();
    util::fullToHomeRel(cPath);
    std::cout << "paradox> " << cPath << " $ ";
}

} // namespace bashkir
