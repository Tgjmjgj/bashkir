#pragma once
#include <string>
#include <vector>

namespace bashkir::builtins
{

class BuiltIn
{
public:
    virtual int exec(std::vector<std::string> arg) = 0;
};

} // namespace builtins
