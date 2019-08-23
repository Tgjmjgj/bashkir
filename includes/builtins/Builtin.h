#pragma once
#include <string>
#include <vector>

namespace bashkir::builtins
{

class BuiltIn
{
public:
    virtual ~BuiltIn() = default;
    virtual int exec(const std::vector<std::string> &arg) = 0;
};

} // namespace bashkir::builtins
