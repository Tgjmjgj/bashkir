#pragma once
#include "parser/interface/ICmdParser.h"

class Shell
{
private:
    ICmdParser* cmdParser;
public:
    Shell();
    ~Shell();
    int run();
private:
    void init();
    void writePrefix();
    std::string waitInput();
};
