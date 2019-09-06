#pragma once
#include <string>
#include <map>
#include <memory>
#include "parser/interface/ICmdParser.h"
#include "builtins/Builtin.h"
#include "input/InputHandler.h"
#include "io/interface/BaseIO.h"
#include "wrappers/NCurses.h"
#include "BuiltinRegistry.h"

namespace bashkir
{

class Shell
{
private:
    std::unique_ptr<ICmdParser> parser;
    std::unique_ptr<InputHandler> input;
    std::shared_ptr<std::vector<std::string>> history;
    std::shared_ptr<BaseIO> io;
    std::shared_ptr<BuiltinRegistry> builtins;
    
public:
    Shell();
    ~Shell();
    int run();

private:
    void init();
    void loadBuiltins();
    void writePrefix() const;
    std::string waitInput() const;
};

} // namespace bashkir
