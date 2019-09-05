#include <iostream>
#include <unistd.h>
#include <experimental/filesystem>
#include "Shell.h"
#include "parser/BashkirCmdParser.h"
#include "exec/Executor.h"
#include "util/pathutil.h"
#include "util/convutil.h"
#include "builtins/cd/cd.h"
#include "builtins/pwd/pwd.h"
#include "builtins/history/history.h"
#include "io/StreamIO.h"

namespace fs = std::experimental::filesystem;

namespace bashkir
{

Shell::Shell()
{
    memset(&this->settings_before, 0, sizeof(termios));
    tcgetattr(0, &this->settings_before);
    if (isatty(STDIN_FILENO))
        setvbuf(stdin, NULL, _IONBF, 0);
    if (isatty(STDOUT_FILENO))
        setvbuf(stdout, NULL, _IONBF, 0);
    if (isatty(STDERR_FILENO))
        setvbuf(stderr, NULL, _IONBF, 0);
    termios settings;
    memset(&settings, 0, sizeof(termios));
    settings.c_cflag |= util::i2ui(CREAD);
    settings.c_cflag |= util::i2ui(CS8);
    settings.c_oflag |= util::i2ui(OPOST);
    settings.c_oflag |= util::i2ui(ONLCR);
    settings.c_cc[VMIN]  = 0;
    settings.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &settings) < 0)
    {
        std::cerr << "Error with setting new term properties.\n";
    }
    fs::current_path(getenv("HOME"));
    this->init();
}

Shell::~Shell()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &this->settings_before);
}

void Shell::init()
{
    this->io = std::make_shared<StreamIO>();
    this->history = std::make_shared<std::vector<std::string>>();
    this->input = std::make_unique<InputHandler>(this->io, this->history);
    this->parser = std::make_unique<BashkirCmdParser>(this->io, this->history);
    this->loadBuiltins();
}

void Shell::loadBuiltins()
{
    const std::shared_ptr<builtins::Cd> cd = std::make_shared<builtins::Cd>(this->io);
    if (this->registerBuiltin("cd", cd) == -1)
    {
        this->io->error("Error with register builtin 'cd'");
    }
    if (this->registerBuiltin("pushd", cd) == -1)
    {
        this->io->error("Error with register builtin 'pushd'");
    }
    if (this->registerBuiltin("popd", cd) == -1)
    {
        this->io->error("Error with register builtin 'popd'");
    }
    if (this->registerBuiltin("pwd", std::make_shared<builtins::Pwd>(this->io)) == -1)
    {
        this->io->error("Error with register builtin 'pwd'");
    }
    if (this->registerBuiltin("history", std::make_shared<builtins::History>(this->io, this->history)) == -1)
    {
        this->io->error("Error with register builtin 'history'");
    }
}

int Shell::registerBuiltin(const std::string &name, const std::shared_ptr<builtins::BuiltIn> handler)
{
    this->builtins.insert_or_assign(name, std::move(handler));
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
        this->writePrefix();
        const std::string inputStr = this->input->waitInput();
        auto cmds = this->parser->parse(inputStr);
        for (const Command &cmd : cmds)
        {
            auto builtin = this->findBuiltin(cmd.exe);
            if (builtin != nullptr)
            {
                builtin.get()->exec(cmd);
            }
            else
            {
                std::unique_ptr<Executor> exec = std::make_unique<Executor>(this->io);
                exec->execute(cmd);
                exec->waitSubproc();
            }
        }
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
