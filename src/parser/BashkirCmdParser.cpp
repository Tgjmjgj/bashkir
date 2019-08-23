#include "parser/BashkirCmdParser.h"
#include "util/strutil.h"
#include <algorithm>
#include <string.h>

using std::string, std::vector;

namespace bashkir
{

vector<Command> BashkirCmdParser::parse(const string &input_str) const
{
    vector<Command> cmds = std::vector<Command>();
    const string union_delim = "&&";
    size_t start_pos, pos = 0l - union_delim.size();
    do
    {
        start_pos = pos + union_delim.size();
        pos = input_str.find(union_delim, start_pos);
        string cmd_str = input_str.substr(start_pos, pos - start_pos);
        util::trim(cmd_str);
        if (cmd_str.size() == 0)
        {
            continue;
        }
        Command cmd;
        std::size_t arg_pos, arg_start_pos = 0;
        string arg;
        do
        {
            arg_pos = cmd_str.find_first_of(' ', arg_start_pos);
            if (arg_pos == string::npos)
            {
                arg = cmd_str.substr(arg_start_pos);
            }
            else
            {
                arg = cmd_str.substr(arg_start_pos, arg_pos - arg_start_pos);
                arg_start_pos = arg_pos + 1;
            }
            if (arg == "")
            {
                continue;
            }
            cmd.args.push_back(arg);
        } while (arg_pos != string::npos);
        cmd.exe = cmd.args[0];
        cmd.args.erase(cmd.args.begin());
        cmds.push_back(cmd);
    } while (pos != string::npos);
    return cmds;
}

} // namespace bashkir
