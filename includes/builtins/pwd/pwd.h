#pragma once
#include <string>
#include <vector>
#include "builtins/Builtin.h"

namespace bashkir::builtins
{

class Pwd : public BuiltIn
{
public:
    ~Pwd() {}
    int exec(const std::vector<std::string> &args);
};

} // namespace bashkir::builtins
