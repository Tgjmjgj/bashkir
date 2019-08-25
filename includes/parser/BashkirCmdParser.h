#pragma once
#include "parser/interface/ICmdParser.h"
#include <memory>

namespace bashkir
{

class BashkirCmdParser : public ICmdParser
{
    std::shared_ptr<std::vector<std::string>> history;
public:
    BashkirCmdParser(std::shared_ptr<std::vector<std::string>> hist);
    ~BashkirCmdParser() {}
    std::vector<Command> parse(const std::string &input_str);
    void postprocess(std::vector<Command> &cmds) const;
    bool substitution(std::string &argument) const;

};

} // namespace bashkir
