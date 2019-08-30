#pragma once
#include <string>
#include <map>
#include <memory>
#include "parser/interface/ICmdParser.h"
#include "builtins/Builtin.h"
#include "input/InputHandler.h"
#include "io/interface/BaseIO.h"

namespace bashkir
{

class Shell
{
private:
    std::unique_ptr<ICmdParser> parser;
    std::unique_ptr<InputHandler> input;
    std::map<std::string, std::shared_ptr<builtins::BuiltIn>> builtins;
    std::shared_ptr<std::vector<std::string>> history;
    std::shared_ptr<BaseIO> io;
public:
    Shell();
    ~Shell();
    int run();

private:
    void init();
    void loadBuiltins();
    int registerBuiltin(const std::string &name, const std::shared_ptr<builtins::BuiltIn> handler);
    std::shared_ptr<builtins::BuiltIn> findBuiltin(const std::string &name) const;
    void writePrefix() const;
    std::string waitInput() const;
};

} // namespace bashkir
