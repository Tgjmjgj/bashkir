#pragma once
#include <deps.h>
// #include <string>
// #include <vector>
// #include <memory>
#include "builtins/Builtin.h"

namespace bashkir::builtins
{

class History : public BuiltIn
{
private:
    std::shared_ptr<std::vector<std::string>> hist;
public:
    History(std::shared_ptr<std::vector<std::string>> history);
    ~History() {}
    int exec(const Command &cmd);
};

} // namespace bashkir::builtins
