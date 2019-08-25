#pragma once
#include "parser/interface/Command.h"
#include <string>
#include <vector>

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
