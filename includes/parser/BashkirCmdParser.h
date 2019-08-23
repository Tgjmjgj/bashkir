#pragma once
#include "parser/interface/ICmdParser.h"

namespace bashkir
{

class BashkirCmdParser : public ICmdParser
{
public:
    BashkirCmdParser() {}
    ~BashkirCmdParser() {}
    std::vector<Command> parse(const std::string &input_str) const;
};

} // namespace bashkir
