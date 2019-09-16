#include <iostream>
#include <unistd.h>
#include <experimental/filesystem>
#include <termios.h>
#include "Shell.h"
#include "global.h"
#include "parser/BashkirCmdParser.h"
#include "exec/Executor.h"
#include "util/pathutil.h"
#include "util/convutil.h"
#include "builtins/cd/cd.h"
#include "builtins/pwd/pwd.h"
#include "builtins/history/history.h"
#include "builtins/exit/exit.h"
#include "builtins/type/type.h"
#include "io/StreamIO.h"


#include <chrono>

namespace fs = std::experimental::filesystem;

namespace bashkir
{

Shell::Shell()
{
    this->io = std::make_shared<StreamIO>();

    memset(&global::settings_classic, 0, sizeof(termios));
    tcgetattr(0, &global::settings_classic);
    if (isatty(STDIN_FILENO))
    {
        setvbuf(stdin, NULL, _IONBF, 0);
    }
    if (isatty(STDOUT_FILENO))
    {
        setvbuf(stdout, NULL, _IONBF, 0);
    }
    if (isatty(STDERR_FILENO))
    {
        setvbuf(stderr, NULL, _IONBF, 0);
    }
    memset(&global::settings_bashkir, 0, sizeof(termios));
    global::settings_bashkir.c_cflag |= util::i2ui(CREAD);
    global::settings_bashkir.c_cflag |= util::i2ui(CS8);
    global::settings_bashkir.c_oflag |= util::i2ui(OPOST);
    global::settings_bashkir.c_oflag |= util::i2ui(ONLCR);
    global::settings_bashkir.c_cc[VMIN]  = 1; // 0
    global::settings_bashkir.c_cc[VTIME] = 0;
    
    if (!global::bashkirTermSettings())
    {
        this->io->error("Error with setting new term properties.");
    }
    atexit(global::atexit);
    signal(SIGCHLD, global::antiZombie);
    fs::current_path(getenv("HOME"));
    this->init();
}

Shell::~Shell() {}

void Shell::init()
{
    this->history = std::make_shared<std::vector<std::string>>();
    this->input = std::make_unique<InputHandler>(this->io, this->history);
    this->parser = std::make_unique<BashkirCmdParser>(this->io, this->history);
    this->builtins = std::make_shared<BuiltinRegistry>();
    this->exec = std::make_unique<ExecManager>(this->io, this->builtins);
    this->loadBuiltins();
}

void Shell::loadBuiltins()
{
    const std::shared_ptr<builtins::Cd> cd = std::make_shared<builtins::Cd>(this->io);
    if (this->builtins->registerBuiltin("cd", cd) == -1)
    {
        this->io->error("Error with register builtin 'cd'");
    }
    if (this->builtins->registerBuiltin("pushd", cd) == -1)
    {
        this->io->error("Error with register builtin 'pushd'");
    }
    if (this->builtins->registerBuiltin("popd", cd) == -1)
    {
        this->io->error("Error with register builtin 'popd'");
    }
    if (this->builtins->registerBuiltin("pwd", std::make_shared<builtins::Pwd>(this->io)) == -1)
    {
        this->io->error("Error with register builtin 'pwd'");
    }
    if (this->builtins->registerBuiltin("history", std::make_shared<builtins::History>(this->io, this->history)) == -1)
    {
        this->io->error("Error with register builtin 'history'");
    }
    if (this->builtins->registerBuiltin("exit", std::make_shared<builtins::Exit>()) == -1)
    {
        this->io->error("Error with register builtin 'exit'");
    }
    if (this->builtins->registerBuiltin("type", std::make_shared<builtins::Type>(this->io, this->builtins)) == -1)
    {
        this->io->error("Error with register builtin 'type'");
    }
}

int Shell::run()
{
    while (true)
    {
        this->writePrefix();
        const std::string inputStr = this->input->waitInput();
        std::vector<Command> cmds = this->parser->parse(inputStr);
        this->exec->execute(cmds);
    }
    return 0;
}

void Shell::writePrefix() const
{
    std::string cPath = fs::current_path().c_str();
    util::fullToHomeRel(cPath);
    this->io->write("paradox> " + cPath + " $ ");
}

} // namespace bashkir
