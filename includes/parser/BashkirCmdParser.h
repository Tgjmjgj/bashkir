#pragma once
#include "parser/interface/ICmdParser.h"

namespace bashkir
{

class BashkirCmdParser : public ICmdParser
{
public:
    BashkirCmdParser() {}
    ~BashkirCmdParser() {}
    std::vector<Command> parse(std::string inputStr);
};

} // namespace bashkir
