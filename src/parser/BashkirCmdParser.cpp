#include "parser/BashkirCmdParser.h"
#include "util/strutil.h"
#include <algorithm>
#include <string.h>

using std::string, std::vector;

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
        vector<char*> args;
        do
        {
            argPos = inputStr.find_first_of(' ', argStartPos);
            if (argPos == string::npos)
                arg = cmdStr.substr(argStartPos);
            else
            {
                arg = cmdStr.substr(argStartPos, argPos - argStartPos);
                argStartPos = argPos + 1;
            }
            char* cArg = new char[arg.length()];
            strcpy(cArg, arg.c_str());
            args.push_back(cArg);
        } while (argPos != string::npos);
        cmd.args = new char*[args.size()];
        std::copy(args.begin(), args.end(), cmd.args);
        cmd.exe = cmd.args[0];
        cmds.push_back(cmd);
    } while (pos != string::npos);
    return cmds;
}
