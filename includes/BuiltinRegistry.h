#pragma once
#include <string>
#include <map>
#include <memory>
#include "builtins/Builtin.h"
#include "io/interface/BaseIO.h"

namespace bashkir
{

class BuiltinRegistry
{
private:
    std::map<std::string, std::shared_ptr<builtins::BuiltIn>> builtins;

public:
    BuiltinRegistry();
    int registerBuiltin(const std::string &name, const std::shared_ptr<builtins::BuiltIn> handler);
    std::shared_ptr<builtins::BuiltIn> findBuiltin(const std::string &name) const;
};

} // namespace bashkir
