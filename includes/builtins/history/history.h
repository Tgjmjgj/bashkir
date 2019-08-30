#pragma once
#include <string>
#include <vector>
#include <memory>
#include "builtins/Builtin.h"
#include "io/interface/BaseIO.h"

namespace bashkir::builtins
{

class History : public BuiltIn
{
private:
    std::shared_ptr<BaseIO> io;
    std::shared_ptr<std::vector<std::string>> hist;
public:
    History(std::shared_ptr<BaseIO> nc_io, std::shared_ptr<std::vector<std::string>> history);
    ~History() {}
    int exec(const Command &cmd);
};

} // namespace bashkir::builtins
