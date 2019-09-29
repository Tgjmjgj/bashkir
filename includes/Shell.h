#pragma once
#include <deps.h>
// #include <string>
// #include <map>
// #include <memory>
#include "parser/interface/ICmdParser.h"
#include "input/InputHandler.h"
#include "exec/ExecManager.h"
#include "builtins/Builtin.h"
#include "wrappers/NCurses.h"
#include "BuiltinRegistry.h"

namespace bashkir
{

class Shell
{
private:
    std::unique_ptr<ICmdParser> parser;
    std::unique_ptr<InputHandler> input;
    std::unique_ptr<ExecManager> exec;
    std::shared_ptr<std::vector<std::string>> history;
    std::shared_ptr<BuiltinRegistry> builtins;
    
public:
    Shell();
    ~Shell();
    int run();

private:
    void init();
    void loadBuiltins();
    void signalHandlers();
    void configureLogger();
    std::string waitInput() const;
};

} // namespace bashkir
