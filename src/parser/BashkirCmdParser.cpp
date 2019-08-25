#include "parser/BashkirCmdParser.h"
#include "util/strutil.h"
#include "util/pathutil.h"
#include <algorithm>
#include <string.h>
#include <iostream>

using std::string, std::vector;

namespace bashkir
{

BashkirCmdParser::BashkirCmdParser(std::shared_ptr<std::vector<std::string>> hist)
    : history(hist) {}

vector<Command> BashkirCmdParser::parse(const string &input_str) const
{
    const string union_delim = "&&";
    vector<Command> cmds = std::vector<Command>();
    auto items = util::items(input_str);
    Command cmd;
    for (auto it = items.begin(); it != items.end(); ++it)
    {
        std::string item = it.getValue();
        std::cout << item << std::endl;

        if (item == union_delim)
        {
            cmds.push_back(cmd);
            cmd = Command();
        }
        else if (cmd.args.empty() && cmd.exe.length() == 0)
        {
            cmd.exe = item;
        }
        else
        {
            cmd.args.push_back(item);
        }
    }
    if (!cmd.args.empty() && cmd.exe.length() != 0)
    {
        cmds.push_back(cmd);
    }
    return cmds;
}

std::string &BashkirCmdParser::substitutions(std::string &argument) const
{
    if (argument[0] == '~')
    {
        util::homeRelToFull(argument);
    }

    return argument;
}

} // namespace bashkir
