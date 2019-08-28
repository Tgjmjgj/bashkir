#pragma once
#include <string>
#include <vector>
#include <memory>
#include "builtins/Builtin.h"

namespace bashkir::builtins
{

class History : public BuiltIn
{
private:
    std::shared_ptr<std::vector<std::string>> history;
public:
    History(std::shared_ptr<std::vector<std::string>> hist);
    ~History() {}
    int exec(const Command &cmd);
};

} // namespace bashkir::builtins
