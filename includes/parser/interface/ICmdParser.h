#pragma once
#include "deps.h"
// #include <string>
// #include <vector>
#include "parser/interface/Command.h"

namespace bashkir
{

class ICmdParser
{
public:
    ICmdParser() {}
    virtual ~ICmdParser() {}

    virtual std::vector<Command> parse(const std::string &input_str) = 0;
};

} // namespace bashkir
