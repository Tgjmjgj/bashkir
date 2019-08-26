#include <string.h>
#include <regex>
#include <cctype>
#include "parser/BashkirCmdParser.h"
#include "parser/ItemsRange.h"
#include "util/pathutil.h"
#include "util/convutil.h"

#include <iostream>

namespace bashkir
{

BashkirCmdParser::BashkirCmdParser(std::shared_ptr<std::vector<std::string>> hist)
    : history(std::move(hist))
{
    std::cout << this->history.use_count() << std::endl;
}

std::vector<Command> BashkirCmdParser::parse(const std::string &input_str)
{
    const std::string union_delim = "&&";
    std::vector<Command> cmds = std::vector<Command>();
    auto items = iterate_items(input_str);
    Command cmd;
    for (auto it = items.begin(); it != items.end(); ++it)
    {
        std::string item = it.getValue();
        if (this->substitution(item))
        {
            it.setValue(item);
            continue;
        }
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
    if (cmd.exe.length() != 0)
    {
        cmds.push_back(cmd);
    }
    this->history->push_back(items.getCompletedCommandString());
    this->postprocess(cmds);
    return cmds;
}

void BashkirCmdParser::postprocess(std::vector<Command> &cmds) const
{
    for (Command &cmd : cmds)
    {
        for (std::string &arg : cmd.args)
        {
            util::tryHomeRelToFull(arg);
        }
    }
}

bool BashkirCmdParser::substitution(std::string &argument) const
{
    std::size_t pos = argument.find('!');
    bool is_changed = false;
    while (pos != std::string::npos && pos != argument.length() - 1)
    {
        switch (argument[pos + 1])
        {
        case '!':
        {
            const std::string last_cmd = this->history->back();
            argument.replace(pos, 2, last_cmd);
            is_changed = true;
            break;
        }
        default:
            break;
        }
        bool reverse = false;
        if (argument[pos + 1] == '-')
        {
            reverse = true;
        }
        const std::size_t rv_shift = reverse ? 2 : 1;
        if (std::isdigit(argument[pos + rv_shift]) != 0)
        {
            std::size_t end = pos + rv_shift;
            while (std::isdigit(argument[end]) != 0)
            {
                end++;
            }
            const std::string str_num = argument.substr(pos + rv_shift, end - pos - rv_shift);
            const std::size_t num = util::int2size_t(std::stoi(str_num));
            const std::size_t hist_index = reverse ? this->history->size() - num : num - 1;
            if (hist_index < this->history->size())
            {
                const std::string hist_item = (*(this->history))[hist_index];
                argument.replace(pos, 1 + str_num.length(), hist_item);
                is_changed = true;
            }
            else
            {
                std::cout << "bashkir: command " << (reverse ? "-" : "") << num
                          << " is not found in history." << std::endl;
            }
        }
        pos = argument.find('!', pos + 1);
    }
    return is_changed;
}

} // namespace bashkir
