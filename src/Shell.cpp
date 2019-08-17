#include <iostream>
#include "parser/BashkirCmdParser.h"
#include "exec/Executor.h"
#include "Shell.h"
#include <sys/wait.h>

Shell::Shell()
{
    this->init();
}

Shell::~Shell()
{
}

void Shell::init()
{
    this->cmdParser = new BashkirCmdParser();
}

int Shell::run()
{
    while (true)
    {
        std::string input = this->waitInput();
        auto cmds = this->cmdParser->parse(input);
        Executor* exec = new Executor();
        exec->execute(cmds[0]);
        wait(NULL);
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
    std::cout << "paradox> ";
}
