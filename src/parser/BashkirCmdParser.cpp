#include "parser/BashkirCmdParser.h"
#include "util/strutil.h"
#include <algorithm>
#include <string.h>

using std::string, std::vector;

namespace bashkir
{

vector<Command> BashkirCmdParser::parse(string inputStr)
{
    vector<Command> cmds;
    cmds = std::vector<Command>();
    const string unionDelim = "&&";
    int startPos, pos = 0l - unionDelim.size();
    do
    {
        startPos = pos + unionDelim.size();
        pos = inputStr.find(unionDelim, startPos);
        string cmdStr = inputStr.substr(startPos, pos - startPos);
        util::trim(cmdStr);
        if (cmdStr.size() == 0)
            continue;
        Command cmd = Command();
        int argPos, argStartPos = 0;
        string arg;
        do
        {
            argPos = cmdStr.find_first_of(' ', argStartPos);
            if (argPos == string::npos)
                arg = cmdStr.substr(argStartPos);
            else
            {
                arg = cmdStr.substr(argStartPos, argPos - argStartPos);
                argStartPos = argPos + 1;
            }
            if (arg == "")
                continue;
            cmd.args.push_back(arg);
        } while (argPos != string::npos);
        cmd.exe = cmd.args[0];
        cmd.args.erase(cmd.args.begin());
        cmds.push_back(cmd);
    } while (pos != string::npos);
    return cmds;
}

} // namespace bashkir
