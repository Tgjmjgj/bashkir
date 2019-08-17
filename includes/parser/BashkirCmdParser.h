#pragma once
#include "parser/interface/ICmdParser.h"

class BashkirCmdParser : public ICmdParser
{
public:
    BashkirCmdParser() {}
    ~BashkirCmdParser() {}
    std::vector<Command> parse(std::string inputStr);
};
