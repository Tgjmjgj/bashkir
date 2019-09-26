#pragma once
#include "deps.h"
// #include <memory>
#include "parser/interface/ICmdParser.h"

namespace bashkir
{

class BashkirCmdParser : public ICmdParser
{
    std::shared_ptr<std::vector<std::string>> hist;
public:
    BashkirCmdParser(std::shared_ptr<std::vector<std::string>> history);
    ~BashkirCmdParser() {}
    std::vector<Command> parse(const std::string &input_str);
    bool substitution(std::string &argument) const;
    bool substituteHist(std::string &argument) const;
    bool substituteEnv(std::string &argument) const;
    bool substituteGlob(std::string &argument) const;
    bool substitutePath(std::string &argument) const;

};

} // namespace bashkir
