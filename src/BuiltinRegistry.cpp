
#include "BuiltinRegistry.h"

namespace bashkir
{

BuiltinRegistry::BuiltinRegistry() {}

int BuiltinRegistry::registerBuiltin(const std::string &name, const std::shared_ptr<builtins::BuiltIn> handler)
{
    this->builtins.insert_or_assign(name, std::move(handler));
    return 0;
}

std::shared_ptr<builtins::BuiltIn> BuiltinRegistry::findBuiltin(const std::string &name) const
{
    auto it = this->builtins.find(name);
    return it == this->builtins.end() ? nullptr : it->second;
}

} // namespace bashkir
