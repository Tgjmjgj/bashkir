#pragma once
#include <string>
#include <map>
#include <memory>
#include "parser/interface/ICmdParser.h"
#include "builtins/Builtin.h"

namespace bashkir
{

class Shell
{
private:
    std::unique_ptr<ICmdParser> parser;
    std::string dir;
    std::map<std::string, std::shared_ptr<builtins::BuiltIn>> builtins;
    std::shared_ptr<std::vector<std::string>> history;
public:
    Shell();
    int run();

private:
    void init();
    int registerBuiltin(const std::string &name, const std::shared_ptr<builtins::BuiltIn> &&handler);
    std::shared_ptr<builtins::BuiltIn> findBuiltin(const std::string &name) const;
    void writePrefix() const;
    std::string waitInput() const;
};

} // namespace bashkir
