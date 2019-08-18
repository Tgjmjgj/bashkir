#pragma once
#include <string>
#include <vector>

namespace builtins
{

class BuiltIn
{
public:
    virtual int exec(std::vector<std::string> arg) = 0;
};

} // namespace builtins
