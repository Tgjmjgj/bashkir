// #include <iostream>
// #include <unistd.h>
// #include <experimental/filesystem>
// #include <termios.h>
// #include <exception>
#include "Shell.h"
#include "global.h"
#include "input/PreParsedInput.h"
#include "parser/BashkirCmdParser.h"
#include "parser/ExecutionTree.h"
#include "io/StdCapture.h"
#include "exec/Executor.h"
#include "util/pathutil.h"
#include "util/convutil.h"
#include "builtins/cd/cd.h"
#include "builtins/pwd/pwd.h"
#include "builtins/history/history.h"
#include "builtins/exit/exit.h"
#include "builtins/type/type.h"
#include "builtins/export/export.h"
#include "logger/SpdFileLogger.h"
#include "exceptions/ExitException.h"

namespace fs = std::experimental::filesystem;

namespace bashkir
{

Shell::Shell()
{
    this->init();
}

Shell::~Shell()
{
    log::to.Flush();
}

void Shell::init()
{
    this->configureLogger();
    this->changeTerminalSettings();
    fs::current_path(getenv("HOME"));
    this->history = std::make_shared<std::vector<std::string>>();
    this->input = std::make_unique<InputHandler>(this->history);
    this->parser = std::make_unique<BashkirCmdParser>(this->history);
    this->builtins = std::make_shared<BuiltinRegistry>();
    this->exec = std::make_unique<ExecManager>(this->builtins);
    this->loadBuiltins();
}

void Shell::changeTerminalSettings()
{
    memset(&global::settings_original, 0, sizeof(termios));
    tcgetattr(0, &global::settings_original);
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

    // We need to call this method 2 times:
    // Before setting new terminal settings,
    this->registerSignalHandlers();
    global::setBashkirTermSettings();
    this->registerSignalHandlers();
    // And after.
    // This allows signal handlers works in both modes:
    // When new settings are set (when user typing commands in terminal)
    // and when original settings restored (when child processes are running)
}

void Shell::loadBuiltins()
{
    const std::shared_ptr<builtins::Cd> cd = std::make_shared<builtins::Cd>();
    if (this->builtins->registerBuiltin("cd", cd) == -1)
    {
        io.error("Error with register builtin 'cd'");
    }
    if (this->builtins->registerBuiltin("pushd", cd) == -1)
    {
        io.error("Error with register builtin 'pushd'");
    }
    if (this->builtins->registerBuiltin("popd", cd) == -1)
    {
        io.error("Error with register builtin 'popd'");
    }
    if (this->builtins->registerBuiltin("pwd", std::make_shared<builtins::Pwd>()) == -1)
    {
        io.error("Error with register builtin 'pwd'");
    }
    if (this->builtins->registerBuiltin("history", std::make_shared<builtins::History>(this->history)) == -1)
    {
        io.error("Error with register builtin 'history'");
    }
    if (this->builtins->registerBuiltin("exit", std::make_shared<builtins::Exit>()) == -1)
    {
        io.error("Error with register builtin 'exit'");
    }
    if (this->builtins->registerBuiltin("type", std::make_shared<builtins::Type>(this->builtins)) == -1)
    {
        io.error("Error with register builtin 'type'");
    }
    if (this->builtins->registerBuiltin("export", std::make_shared<builtins::Export>()) == -1)
    {
        io.error("Error with register builtin 'export'");
    }
}

void Shell::registerSignalHandlers()
{
    atexit(global::atexit);
    // Disable Ctrl+C exit
    signal(SIGINT, SIG_IGN);
    // signal(SIGCHLD, global::antiZombie);
}

void Shell::configureLogger()
{
    log::log_level = 0;
}

int Shell::run()
{
    uint8_t bad_alloc_chain = 0;
    while (true)
    {
        try
        {
            const PreParsedInput inpt = this->input->waitInput();
            ExecutionTree etree;
            etree.buildTree(inpt);
            while (!etree.isCompletedCommand())
            {
                auto proc_unit = etree.getNextUnit();
                std::vector<Command> cmds = this->parser->parse(proc_unit->value);
                StdCapture::BeginCapture();
                int exit_code = this->exec->execute(cmds);
                StdCapture::EndCapture();
                std::string output = StdCapture::GetCapture();
                etree.setInnerCommandResult(proc_unit, output);
            }
            auto proc_unit = etree.getNextUnit();
            std::vector<Command> cmds = this->parser->parse(proc_unit->value);
            this->exec->execute(cmds);
            bad_alloc_chain = 0;
        }
        catch (const std::bad_alloc &e)
        {
            log::to.Err(e.what());
            if (bad_alloc_chain > 2) 
            {
                log::to.Err("The memory allocation problem occured 3 times in a row.");
                return 1;
            }
            ++bad_alloc_chain;
        }
        catch (const exc::ExitException &e)
        {
            return e.exitCode();
        }
        catch (const std::exception &e)
        {
            log::to.Err(e.what());
        }
    }
    return 0;
}

} // namespace bashkir
