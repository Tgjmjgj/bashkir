#pragma once
#include "parser/interface/Command.h"
#include <string>
#include <vector>

class ICmdParser
{
public:
    ICmdParser() {}
    virtual ~ICmdParser() {}

    virtual std::vector<Command> parse(std::string inputStr) = 0;
};
